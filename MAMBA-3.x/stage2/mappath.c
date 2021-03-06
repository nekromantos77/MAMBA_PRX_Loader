#include <lv2/lv2.h>
#include <lv2/libc.h>
#include <lv2/memory.h>
#include <lv2/patch.h>
#include <lv2/syscall.h>
#include <lv2/io.h>
#include <lv2/error.h>
#include "common.h"
#include "mappath.h"
#include "modulespatch.h"
#include "syscall8.h"

#define MAX_TABLE_ENTRIES 24

typedef struct _MapEntry
{
	char *oldpath;
	char *newpath;
	int16_t oldpath_len;
	int16_t newpath_len;
	uint32_t flags;
} MapEntry;

MapEntry map_table[MAX_TABLE_ENTRIES];


#define _MAX_PATH    512

// TODO: map_path and open_path_hook should be mutexed...

int8_t max_table_entries = 0;
int8_t condition_apphome_index = 0;

int map_path(char *oldpath, char *newpath, uint32_t flags)
{
	int8_t i, firstfree = -1, is_dev_bdvd = 0;

	if (!oldpath || *oldpath == 0)
		return -1;

	#ifdef  DEBUG
	DPRINTF("Map path: %s -> %s\n", oldpath, newpath);
	#endif

	if (newpath && strcmp(oldpath, newpath) == 0)
		newpath = NULL;

	if (strcmp(oldpath, "/dev_bdvd") == 0)
	{
		condition_apphome = (newpath != NULL);
		condition_apphome_index = 0, is_dev_bdvd = 1; //AV 20190405
	}

	for (i = 0; i < max_table_entries; i++)
	{
		if (map_table[i].oldpath)
		{
			if (strcmp(oldpath, map_table[i].oldpath) == 0)
			{
				if (newpath && (*newpath != 0))
				{
					int len = strlen(newpath); if(len > _MAX_PATH) return -1;

					// replace mapping
					strncpy(map_table[i].newpath, newpath, len);
					map_table[i].newpath[len] = 0;
					map_table[i].newpath_len = len;
					map_table[i].flags = (map_table[i].flags&FLAG_COPY) | (flags&(~FLAG_COPY));

					if(is_dev_bdvd) condition_apphome_index = i;
				}
				else
				{
					// delete mapping
					if (map_table[i].flags & FLAG_COPY)
						dealloc(map_table[i].oldpath, 0x27);

					dealloc(map_table[i].newpath, 0x27);
					map_table[i].oldpath = NULL;
					map_table[i].newpath = NULL;
					map_table[i].oldpath_len = 0;
					map_table[i].newpath_len = 0;
					map_table[i].flags = 0;

					for(int8_t n = max_table_entries - 1; n >= 0; n--) {if(map_table[n].oldpath) break; else if(max_table_entries > 0) max_table_entries--;}
				}

				return 0;
			}
		}
		else if (firstfree < 0)
		{
			firstfree = i;
		}
	}

	if (firstfree < 0) firstfree = max_table_entries;

	// add new mapping
	if (firstfree < MAX_TABLE_ENTRIES)
	{
		if (!newpath || *newpath == 0)
			return 0;

		map_table[firstfree].flags = flags;

		int len = strlen(oldpath);
		map_table[firstfree].oldpath_len = len;

		if (flags & FLAG_COPY)
		{
			map_table[firstfree].oldpath = alloc(len + 1, 0x27);
			strncpy(map_table[firstfree].oldpath, oldpath, len);
			map_table[firstfree].oldpath[len] = 0;
		}
		else
		{
			map_table[firstfree].oldpath = oldpath;
		}

		map_table[firstfree].newpath = alloc(_MAX_PATH + 1, 0x27);
		strncpy(map_table[firstfree].newpath, newpath, _MAX_PATH);
		map_table[firstfree].newpath[_MAX_PATH] = 0;
		map_table[firstfree].newpath_len = strlen(newpath);

		if(is_dev_bdvd) condition_apphome_index = firstfree;

		max_table_entries++;
		return 0;
	}

	return -1; // table entries is full
}

int map_path_user(char *oldpath, char *newpath, uint32_t flags)
{
	char *oldp, *newp;

	#ifdef  DEBUG
	DPRINTF("map_path_user, called by process %s: %s -> %s\n", get_process_name(get_current_process_critical()), oldpath, newpath);
	#endif

	if (oldpath == 0)
		return -1;

	int ret = pathdup_from_user(get_secure_user_ptr(oldpath), &oldp);
	if (ret != 0)
		return ret;

	if (newpath == 0)
	{
		newp = NULL;
	}
	else
	{
		ret = pathdup_from_user(get_secure_user_ptr(newpath), &newp);
		if (ret != 0)
		{
			dealloc(oldp, 0x27);
			return ret;
		}
	}

	ret = map_path(oldp, newp, flags | FLAG_COPY);

	dealloc(oldp, 0x27);
	if (newp)
		dealloc(newp, 0x27);

	return ret;
}

LV2_SYSCALL2(int, sys_map_path, (char *oldpath, char *newpath))
{
	extend_kstack(0);
	return map_path_user(oldpath, newpath, 0);
}

int sys_map_paths(char *paths[], char *new_paths[], unsigned int num)
{
	uint32_t *u_paths = (uint32_t *)get_secure_user_ptr(paths);
	uint32_t *u_new_paths = (uint32_t *)get_secure_user_ptr(new_paths);

	int unmap = 0;
	int ret = 0;

	if (!u_paths)
	{
		unmap = 1;
	}
	else
	{
		if (!u_new_paths)
			return EINVAL;

		for (unsigned int i = 0; i < num; i++)
		{
			ret = map_path_user((char *)(uint64_t)u_paths[i], (char *)(uint64_t)u_new_paths[i], FLAG_TABLE);
			if (ret != 0)
			{
				unmap = 1;
				break;
			}
		}
	}

	if (unmap)
	{
		for (int8_t i = 0; i < max_table_entries; i++)
		{
			if (map_table[i].flags & FLAG_TABLE)
			{
				if (map_table[i].flags & FLAG_COPY)
					dealloc(map_table[i].oldpath, 0x27);

				dealloc(map_table[i].newpath, 0x27);
				map_table[i].oldpath = NULL;
				map_table[i].newpath = NULL;
				map_table[i].flags = 0;
			}
		}

		max_table_entries = 0;
	}

	return ret;
}
//////////////////////////////////////////////////////////////////////////////////////
// KW - HOMEBREW BLOCKER SUPPORT CODE TO USE IN open_path_hook()
//
// Functions, global vars and directives are here to improve code readability
//

#define BLACKLIST_FILENAME "/dev_hdd0/tmp/blacklist.cfg"
#define WHITELIST_FILENAME "/dev_hdd0/tmp/whitelist.cfg"
#define MAX_LIST_ENTRIES 30 // Maximum elements for noth the custom blacklist and whitelist.

static int __initialized_lists = 0; // Are the lists initialized ?
static int __blacklist_entries = 0; // Module global var to hold the current blacklist entries.
static char __blacklist[9*MAX_LIST_ENTRIES];
static int __whitelist_entries = 0; // Module global var to hold the current whitelist entries.
static char __whitelist[9*MAX_LIST_ENTRIES];

static int read_text_line(int fd, char *line, unsigned int size, int *eof)
{
	int i = 0;
	int line_started = 0;

	if (size == 0)
		return -1;

	*eof = 0;

	while (i < (size-1))
	{
		uint8_t ch;
		uint64_t r;

		if (cellFsRead(fd, &ch, 1, &r) != 0 || r != 1)
		{
			*eof = 1;
			break;
		}

		if (!line_started)
		{
			if (ch > ' ')
			{
				line[i++] = (char)ch;
				line_started = 1;
			}
		}
		else
		{
			if (ch == '\n' || ch == '\r')
				break;

			line[i++] = (char)ch;
		}
	}

	line[i] = 0;

	// Remove space chars at end
	for (int j = i-1; j >= 0; j--)
	{
		if (line[j] <= ' ')
		{
			line[j] = 0;
			i = j;
		}
		else
		{
			break;
		}
	}

	return i;
}

//
// init_list()
//
// inits a list.
// returns the number of elements read from file

static int init_list(char *list, char *path, int maxentries)
{
	int loaded, f;

	if (cellFsOpen(path, CELL_FS_O_RDONLY, &f, 0, NULL, 0) != 0)
	return 0; // failed to open
	loaded = 0;
	while (loaded < maxentries)
	{
		char line[128];
		int eof;
		if (read_text_line(f, line, sizeof(line), &eof) > 0)
			if (strlen(line) >=9) // avoid copying empty lines
			{
				strncpy(list + (9*loaded), line, 9); // copy only the first 9 chars - if it has lees than 9, it will fail future checks. should correct in file.
				loaded++;
			}
		if (eof) break;
	}
	cellFsClose(f);
	return loaded;
}


//
// listed()
//
// tests if a char gameid[9] is in the blacklist or whitelist
// initialize the both lists, if not yet initialized;
// receives the list to test blacklist (1) or whitelist (0), and the gameid
// to initialize the lists, tries to read them from file BLACKLIST_FILENAME and WHITELIST_FILENAME

static int listed(int blacklist, char *gameid)
{
	char *list;
	int i, elements;
	if (!__initialized_lists)
	{
		// initialize the lists if not yet done
		__blacklist_entries = init_list(__blacklist, BLACKLIST_FILENAME, MAX_LIST_ENTRIES);
		__whitelist_entries = init_list(__whitelist, WHITELIST_FILENAME, MAX_LIST_ENTRIES);
		__initialized_lists = 1;
	}
	if (blacklist)
		{list = __blacklist; elements = __blacklist_entries;}
	else
		{list = __whitelist; elements = __whitelist_entries;}

	for (i = 0; i < elements; i++)
		if (!strncmp(list+(9*i),gameid, 9))
			return 1; // gameid is in the list

	// if it got here, it is not in the list. return 0
	return 0;
}


// BEGIN KW & AV block access to homebrews when syscalls are disabled
// After the core tests it will test first if the gameid is in whitelist.cfg (superseeds previous tests)
// In the it will test if the gameid is in blacklist.cfg (superseeds all previous tests)
// ** WARNING ** This syscall disablement test assumes that the syscall table entry 6 (peek) was replaced by the original value (equals syscall 0 entry) as done by PSNPatch
// ** WARNING ** If only a parcial disablement was made, this assumption WILL FAIL !!!

LV2_HOOKED_FUNCTION_POSTCALL_2(void, open_path_hook, (char *path0, int mode))
{
	int syscalls_disabled = ((*(uint64_t *)MKA(syscall_table_symbol + 8 * 6)) == (*(uint64_t *)MKA(syscall_table_symbol)));

	if (syscalls_disabled && path0 && !strncmp(path0, "/dev_hdd0/game/", 15) && strstr(path0 + 15, "/EBOOT.BIN"))
	{
	// syscalls are disabled and an EBOOT.BIN is being called from hdd. Let's test it.
	char *gameid = path0 + 15;

	// flag "whitelist" id's
	int allow =
	!strncmp(gameid, "NP", 2) ||
	!strncmp(gameid, "BL", 2) ||
	!strncmp(gameid, "BC", 2) ||
	!strncmp(gameid, "_INST_", 6) || // 80010006 error fix when trying to install a game update with syscall disabled. # Joonie's, Alexander's, Aldo's
	!strncmp(gameid, "_DEL_", 5) ||  // Fix data corruption if you uninstall game/game update/homebrew with syscall disabled # Alexander's
	!strncmp(gameid, "KOEI3", 5) ||
	!strncmp(gameid, "KTGS3", 5) ||
	!strncmp(gameid, "MRTC0", 5) ||
	!strncmp(gameid, "ASIA0", 5) ||
	!strncmp(gameid, "GUST0", 5) ;

	// flag some "blacklist" id's
	if (
		!strncmp(gameid, "BLES806", 7) || // Multiman and assorted tools are in the format BLES806**
		!strncmp(gameid, "BLJS10018", 9) || // PSNPatch Stealth (older versions were already detected as non-NP/BC/BL)
		!strncmp(gameid, "BLES08890", 9) || // PSNope by user
		!strncmp(gameid, "BLES13408", 9) || // FCEU NES Emulator
		!strncmp(gameid, "BLES01337", 9) || // Awesome File Manager
		!strncmp(gameid, "BLND00001", 9) || // dev_blind
		!strncmp(gameid, "NPEA90124", 9) //|| // SEN Enabler
		//!strcmp (path0, "/dev_bdvd/PS3_UPDATE/PS3UPDAT.PUP") //bluray disk updates
		) allow = 0;

		// test whitelist.cfg and blacklist.cfg
		if (listed(0, gameid)) // whitelist.cfg test
			allow = 1;
		if (listed(1, gameid)) // blacklist.cfg test
			allow = 0;

		// let's now block homebrews if the "allow" flag is false
		if (!allow)
		{
			const char new_path[12] = "/no_exists";
			set_patched_func_param(1, (uint64_t)new_path);
			return;
		}
	}

	/*if(path0[7]=='v')// && map_table[0].newpath)
	{
		if(!map_table[0].newpath) map_table[0].newpath = alloc(0x400, 0x27);
		strcpy(map_table[0].newpath, (char*)"/dev_hdd0/GAMES/BLES01674");
		strcpy(map_table[0].newpath+25, path0+9);
		DPRINTF(">: [%s]\n", map_table[0].newpath);
		set_patched_func_param(1, (uint64_t)map_table[0].newpath);
	}*/


	if (*path0 == '/')
	{
		char *path = path0;
		if(path[1] == '/') path++; if(!path) return;

		// Disabled due to the issue with multiMAN can't copy update files from discs.
		/*if (path && ((strcmp(path, "/dev_bdvd/PS3_UPDATE/PS3UPDAT.PUP") == 0)))  // Blocks FW update from Game discs!
		{
			char not_update[40];
			sprintf(not_update, "/dev_bdvd/PS3_NOT_UPDATE/PS3UPDAT.PUP");
			set_patched_func_param(1, (uint64_t)not_update);
			#ifdef  DEBUG
			DPRINTF("Update from disc blocked!\n");
			#endif
			return;
		}
		*/

		if(*path == '/')
		{
			//DPRINTF("?: [%s]\n", path);

			for (int8_t i = max_table_entries - 1; i >= 0; i--)
			{
				if (map_table[i].oldpath)
				{
					int16_t len = map_table[i].oldpath_len;

					if(strncmp(path, map_table[i].oldpath, len) == 0)
					{
						strcpy(map_table[i].newpath + map_table[i].newpath_len, path + len);

						// -- AV: use partial folder remapping when newpath starts with double '/' like //dev_hdd0/blah...
						if(map_table[i].newpath[1] == '/')
						{
							CellFsStat stat;
							if (cellFsStat(map_table[i].newpath, &stat) != 0)
							{
								#ifdef  DEBUG
								DPRINTF("open_path %s\n", path0);
								#endif
								return; // Do not remap / Use the original file when redirected file does not exist
							}
						}

						set_patched_func_param(1, (uint64_t)map_table[i].newpath);

						#ifdef  DEBUG
						DPRINTF("open_path %s\n", map_table[i].newpath);
						#endif
						return;
					}
				}
			}
		}

		#ifdef  DEBUG
		DPRINTF("open_path %s\n", path0);
		#endif
	}
}

int sys_aio_copy_root(char *src, char *dst)
{
	int len;

	src = get_secure_user_ptr(src);
	dst = get_secure_user_ptr(dst);

	// Begin original function implementation
	if (!src)
		return EFAULT;

	len = strlen(src);

	if (len > _MAX_PATH || len <= 1 || src[0] != '/')
		return EINVAL;

	strcpy(dst, src);

	// Get device name
	for (int i = 1; i < len; i++)
	{
		if (dst[i] == 0)
			break;

		if (dst[i] == '/')
		{
			dst[i] = 0;
			break;
		}

		if(i >= 0x10) return EINVAL;
	}

	// Here begins custom part of the implementation
	if (condition_apphome && (strcmp(dst, "/dev_bdvd") == 0)) // if dev_bdvd and jb game mounted
	{
		// find /dev_bdvd
		for (int8_t i = condition_apphome_index; i < max_table_entries; i++)
		{
			if (map_table[i].oldpath && strcmp(map_table[i].oldpath, "/dev_bdvd") == 0)
			{
				for (int j = 1; j < map_table[i].newpath_len; j++)
				{
					dst[j] = map_table[i].newpath[j];

					if (dst[j] == 0)
						break;

					if (dst[j] == '/')
					{
						dst[j] = 0;
						break;
					}
				}

				#ifdef  DEBUG
				DPRINTF("AIO: root replaced by %s\n", dst);
				#endif

				break;
			}
		}
	}

	return 0;
}

#ifdef PS3M_API
void unhook_all_map_path(void)
{
	suspend_intr();
	unhook_function_with_postcall(open_path_symbol, open_path_hook, 2);
	resume_intr();
}
#endif

void map_path_patches(int syscall)
{
	hook_function_with_postcall(open_path_symbol, open_path_hook, 2);

	if (syscall) create_syscall2(SYS_MAP_PATH, sys_map_path);
}

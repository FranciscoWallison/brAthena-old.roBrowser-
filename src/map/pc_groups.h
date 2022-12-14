/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/map/pc_groups.h                                                  *
* Descri??o Prim?ria.                                                        *
* Descri??o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena, eAthena                                          *
* \date ?                                                                    *
* \todo ?                                                                    *  
*****************************************************************************/

#ifndef _PC_GROUPS_H_
#define _PC_GROUPS_H_

/// PC permissions
enum e_pc_permission {
	PC_PERM_NONE                = 0,
	PC_PERM_TRADE               = 0x000001,
	PC_PERM_PARTY               = 0x000002,
	PC_PERM_ALL_SKILL           = 0x000004,
	PC_PERM_USE_ALL_EQUIPMENT   = 0x000008,
	PC_PERM_SKILL_UNCONDITIONAL = 0x000010,
	PC_PERM_JOIN_ALL_CHAT       = 0x000020,
	PC_PERM_NO_CHAT_KICK        = 0x000040,
	PC_PERM_HIDE_SESSION        = 0x000080,
	PC_PERM_WHO_DISPLAY_AID     = 0x000100,
	PC_PERM_RECEIVE_HACK_INFO   = 0x000200,
	PC_PERM_WARP_ANYWHERE       = 0x000400,
	PC_PERM_VIEW_HPMETER        = 0x000800,
	PC_PERM_VIEW_EQUIPMENT      = 0x001000,
	PC_PERM_USE_CHECK           = 0x002000,
	PC_PERM_USE_CHANGEMAPTYPE   = 0x004000,
	PC_PERM_USE_ALL_COMMANDS    = 0x008000,
	PC_PERM_RECEIVE_REQUESTS    = 0x010000,
	PC_PERM_SHOW_BOSS           = 0x020000,
	PC_PERM_DISABLE_PVM         = 0x040000,
	PC_PERM_DISABLE_PVP         = 0x080000,
	PC_PERM_DISABLE_CMD_DEAD    = 0x100000,
	PC_PERM_NO_TAKE_ITEM        = 0x200000,    // [Holy]
	PC_PERM_DISABLE_SHOP        = 0x400000,    // [Megasantos]
	PC_PERM_DISABLE_DROPS       = 0x800000,    // [Megasantos]
	PC_PERM_DISABLE_EXP    	    = 0x1000000,   // [Megasantos]
	PC_PERM_NOT_ATTACK    	    = 0x2000000,   // [Holy]
	PC_PERM_NO_CREATE_GUILD     = 0x4000000,   // [Holy]
	PC_PERM_NOT_USE_SKILL       = 0x8000000,   // [Holy]
	PC_PERM_CHANNEL_ADMIN       = 0x10000000,
	PC_PERM_TRADE_BOUND         = 0x20000000,
};

// Cached config settings for quick lookup
struct GroupSettings {
	unsigned int id; // groups.[].id
	int level; // groups.[].level
	char *name; // copy of groups.[].name
	unsigned int e_permissions; // packed groups.[].permissions
	bool log_commands; // groups.[].log_commands
	int index; // internal index of the group (contiguous range starting at 0) [Ind]
	/// Following are used/available only during config reading
	config_setting_t *commands; // groups.[].commands
	config_setting_t *permissions; // groups.[].permissions
	config_setting_t *inherit; // groups.[].inherit
	bool inheritance_done; // have all inheritance rules been evaluated?
	config_setting_t *root; // groups.[]
};

struct pc_groups_permission_table {
	char *name;
	unsigned int permission;
};

typedef struct GroupSettings GroupSettings;

struct pc_groups_interface {
	/* */
	DBMap* db; // id -> GroupSettings
	DBMap* name_db; // name -> GroupSettings
	/* */
	struct pc_groups_permission_table *permissions;
	unsigned char permission_count;
	/* */
	void (*init) (void);
	void (*final) (void);
	void (*reload) (void);
	/* */
	GroupSettings* (*get_dummy_group) (void);
	bool (*exists) (int group_id);
	GroupSettings* (*id2group) (int group_id);
	bool (*has_permission) (GroupSettings *group, unsigned int permission);
	bool (*should_log_commands) (GroupSettings *group);
	const char* (*get_name) (GroupSettings *group);
	int (*get_level) (GroupSettings *group);
	int (*get_idx) (GroupSettings *group);
	unsigned int (*add_permission) (const char *name);
};

struct pc_groups_interface *pcg;

void pc_groups_defaults(void);

#endif // _PC_GROUPS_H_

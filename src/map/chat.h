/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/map/chat.h                                                       *
* Descri??o Prim?ria.                                                        *
* Descri??o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena, eAthena                                          *
* \date ?                                                                    *
* \todo ?                                                                    *  
*****************************************************************************/

#ifndef _CHAT_H_
#define _CHAT_H_

#include "map.h" // struct block_list, CHATROOM_TITLE_SIZE
struct map_session_data;
struct chat_data;

#define MAX_CHAT_USERS 20

struct chat_data {
	struct block_list bl;            // data for this map object
	char title[CHATROOM_TITLE_SIZE]; // room title
	char pass[CHATROOM_PASS_SIZE];   // password
	bool pub;                        // private/public flag
	uint8 users;                     // current user count
	uint8 limit;                     // join limit
	uint8 trigger;                   // number of users needed to trigger event
	uint32 zeny;                         // required zeny to join
	uint32 minLvl;                   // minimum base level to join
	uint32 maxLvl;                   // maximum base level allowed to join
	struct map_session_data *usersd[MAX_CHAT_USERS];
	struct block_list *owner;
	char npc_event[EVENT_NAME_LENGTH];
	DBMap *kick_list;               //DBMap of users who were kicked from this chat
};


bool chat_createpcchat(struct map_session_data *sd, const char *title, const char *pass, int limit, bool pub);
bool chat_joinchat(struct map_session_data *sd, int chatid, const char *pass);
int chat_leavechat(struct map_session_data *sd, bool kicked);
bool chat_changechatowner(struct map_session_data *sd, const char *nextownername);
bool chat_changechatstatus(struct map_session_data *sd, const char *title, const char *pass, int limit, bool pub);
bool chat_kickchat(struct map_session_data *sd, const char *kickusername);
bool chat_createnpcchat(struct npc_data *nd, const char *title, int limit, bool pub, int trigger, const char *ev, int zeny, int minLvl, int maxLvl);
bool chat_deletenpcchat(struct npc_data *nd);
bool chat_enableevent(struct chat_data *cd);
bool chat_triggerevent(struct chat_data *cd);
bool chat_disableevent(struct chat_data *cd);
bool chat_npckickall(struct chat_data *cd);

#endif /* _CHAT_H_ */

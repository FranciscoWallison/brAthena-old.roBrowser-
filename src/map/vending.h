/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/map/vending.h                                                    *
* Descri??o Prim?ria.                                                        *
* Descri??o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena, eAthena                                          *
* \date ?                                                                    *
* \todo ?                                                                    *  
*****************************************************************************/

#ifndef	_VENDING_H_
#define	_VENDING_H_

#include "../common/cbasetypes.h"
#include "../common/db.h"
struct map_session_data;
struct s_search_store_search;

struct s_vending {
	short index; //cart index (return item data)
	short amount; //amout of the item for vending
	unsigned int value; //at wich price
};

struct vending_interface {
	unsigned int next_id;/* next vender id */
	DBMap *db;
	/* */
	void (*init) (void);
	void (*final) (void);
	/* */
	void (*close) (struct map_session_data* sd);
	void (*open) (struct map_session_data* sd, const char* message, const uint8* data, int count);
	void (*list) (struct map_session_data* sd, unsigned int id);
	void (*purchase) (struct map_session_data* sd, int aid, unsigned int uid, const uint8* data, int count);
	bool (*search) (struct map_session_data* sd, unsigned short nameid);
	bool (*searchall) (struct map_session_data* sd, const struct s_search_store_search* s);
};

struct vending_interface *vending;

void vending_defaults(void);

#endif /* _VENDING_H_ */

/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/char/int_mercenary.h                                             *
* Descri??o Prim?ria.                                                        *
* Descri??o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena, eAthena                                          *
* \date ?                                                                    *
* \todo ?                                                                    *  
*****************************************************************************/ 

#ifndef _INT_MERCENARY_SQL_H_
#define _INT_MERCENARY_SQL_H_

struct s_mercenary;

int inter_mercenary_sql_init(void);
void inter_mercenary_sql_final(void);
int inter_mercenary_parse_frommap(int fd);

// Mercenary Owner Database
bool mercenary_owner_fromsql(int char_id, struct mmo_charstatus *status);
bool mercenary_owner_tosql(int char_id, struct mmo_charstatus *status);
bool mercenary_owner_delete(int char_id);

bool mapif_mercenary_delete(int merc_id);

#endif /* _INT_MERCENARY_SQL_H_ */

#include <stdio.h>
#include <ipcs/config.h>
#include <ipcs/msgs.h>
#include <ipcs/links.h>
#include <ipcs/when.h>
#include <ipcs/mailclass.h>
#include <ipcs/events.h>
#include <ipcs/msgerror.h>
#include <mdsevents.h>




static int debug = 0;




mdswtevent(event,data,dlen)
char *event;
char *data;
int *dlen;

{
    short class[class_size];
    char line[4097];
    int status;
    int hiddennoop();
    idptr id;

    if((id = find_mdsevent_id(event,NULL)) == NULL){
        id = create_mdsevent_id(event);
        memset(class,0L,sizeof(class));
        class[major_class] = mc_events;
        class[minor_class] = mc_wait;
        class[private_tag] = id->tag;
        sprintf(line,"%s\n0\n0\n",event);
        ipcs_sendmsg(MDSEVENTD,class,line,strlen(line));
    }
    if(id && id->status != 0){	/* 	event already occured  */
				if(dlen && data){
						if(*dlen > id->dlen)*dlen = id->dlen;
						memcpy(data,id->data,*dlen);
        }
				id->status = 0;
				return;
		}
    if(id && dlen && data){
        id->dlen = *dlen;
    }


    status = mds_handle_event_msg(id);
    if(id && dlen && data){
        memcpy(data,id->data,*dlen);
    }	

}

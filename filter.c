#include "main.h"

HOST_LIST   B_HOST[MAX_FILTER_HOST];
UINT        B_HOST_len = 0;

void UpdateFilterNR()
{
   UCHAR temp[64];
   UINT active = 0;
   int n, Items;

   Items = (int)SendMessage(hFilter,LVM_GETITEMCOUNT,0,0);
   if(Items < 1)
   {
        ShowFilterStatus("Filters: 0 / 0");
        return;
   }
   for(n=0;n<Items;n++)
   {
       if(ListView_GetCheckState(hFilter,n)==1)
         active++;
   }             
   sprintf(temp,"Filters: %u / %i [ %u ]\0",active,Items, B_HOST_len);
   ShowFilterStatus(temp);  
}

UINT IsHostInList(UCHAR *name)
{
    UINT i;
    WANT_READ; //wait if MainThread->WRITE
    for(i=0;i<B_HOST_len;i++)
    {
       if(strstr(name,B_HOST[i].name) != NULL)
              return i;
    }   
    return 0xFFFF;   
}

UCHAR IsHostBlackList(UCHAR *name)
{
    UINT i;
    WANT_READ; //wait if MainThread->WRITE
    for(i=0;i<B_HOST_len;i++)
    {
       if(B_HOST[i].block[0] == 'B')
       {
          if(strstr(name,B_HOST[i].name) != NULL)
              return YES;
       }
    }   
    return NO;   
}

int BlackListAddHost(UCHAR *name)
{
   int id;
   WANT_READ;  //wait if other threads->READ
   LOCK_READ;
   if(B_HOST_len > (MAX_FILTER_HOST-2))  //30
   {
        UNLOCK_READ;           
        return; 
   }
   id =  ListViewAddHost(" "); //just add it,it shouldn't fail,but who knows
   if(id >= 0)
   {
      memset(B_HOST[id].name,0,32);
      strncpy(B_HOST[id].name,name,31);
      ListViewChangeNameHost(id,B_HOST[id].name);
      B_HOST[id].block[0] = 'N';
      B_HOST_len = id+1;
   }
   UpdateFilterNR();
   UNLOCK_READ;
   return id;   
}


void BlackListRemoveHost(int index)
{
   int last = B_HOST_len -1;
   WANT_READ;
   LOCK_READ;
   if(last == 0) //have just one in list,no sense just delete file NOOO!!!
   {
      B_HOST_len = 0;
      SendMessage(hFilter,LVM_DELETEALLITEMS,(WPARAM)0,(LPARAM)0);  
      UNLOCK_READ;              
      return;
   }
   else if(last != index) //the item to be removet is the last one
   {
        memset(B_HOST[index].name,0,32);     //zeror the item to be removed
        strcpy(B_HOST[index].name,B_HOST[last].name); //copy the last one to this
        ListViewChangeNameHost(index,B_HOST[index].name);     //change the name in listview
        //find if the jump item is checked
        if(ListView_GetCheckState(hFilter, last) == 1) //checked
        {
              ListView_SetCheckState(hFilter, index,1);
        }
        else
        {                                           
           ListView_SetCheckState(hFilter, index,0);
        }  
   }
   B_HOST_len = last;  //reset counter list   
   memset(B_HOST[last].name,0,32);
   ListView_DeleteItem(hFilter,last); 
   UpdateFilterNR();  
   UNLOCK_READ;
}


void BlackListChangeHost(int index,UCHAR *name)
{
     WANT_READ;
     LOCK_READ;
     memset(B_HOST[index].name,0,32);
     strncpy(B_HOST[index].name,name,31); 
     ListViewChangeNameHost(index,B_HOST[index].name); 
     UNLOCK_READ;                                                         
}

void BlackListSetCheckHost(int index,UCHAR l)
{
     B_HOST[index].block[0] = l;
     UpdateFilterNR();
}

UCHAR *BlackListGetHost(int index)
{
     return B_HOST[index].name;
}

//extern CALL ReadFile to LIST
void FileToHost(UCHAR *data)
{
    HOST_LIST *T = (HOST_LIST*)data;
    int ret;
    int id;
    //reset list,new bat list or new dir list
    SendMessage(hFilter,LVM_DELETEALLITEMS,(WPARAM)0,(LPARAM)0); 
    B_HOST_len = 0;
    memset(B_HOST,0,sizeof(B_HOST));
    for(ret=0;ret<MAX_FILTER_HOST;ret++)
    {
        if(T[ret].name[0] != 0)
        {
            id =  BlackListAddHost(T[ret].name); 
            if(T[id].block[0] == 'B')
            {
                   ListView_SetCheckState(hFilter, id,1);
            }
            continue;
        }
        break;
    }  
    UpdateFilterNR();
}

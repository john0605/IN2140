#include "allocation.h"
#include "inode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BLOCKSIZE 4096

int globalIdCount = 0;

struct inode* create_file(struct inode* parent, char* name, char readonly, int size_in_bytes) {
    //Sjekke om det er en fil, for kataloger er dette alltid 0
    if (parent->filesize != 0) {
        perror("Parent is not a directory");
        exit(EXIT_FAILURE);
    }
    //Katalognavn må være unik
    if (find_inode_by_name(parent, name) != NULL) {
        perror("Already exists");
        return NULL;
    }
    struct inode* new_file = malloc(sizeof(struct inode));

    new_file->name = malloc(sizeof(name) + 1);
    strcpy(new_file->name, name);
    new_file->id = globalIdCount;
    globalIdCount++;

    new_file->is_directory = 0;
    new_file->is_readonly = readonly;
    new_file->filesize = size_in_bytes;
    new_file->num_entries = (size_in_bytes + BLOCKSIZE -1) / BLOCKSIZE;
    
    size_t entries = new_file->num_entries * sizeof(struct inode);
    new_file->entries = malloc(entries);
    
    for (int i = 0; i < new_file->num_entries; i++) {
        new_file->entries[i] = allocate_block();
    }
    
    if (parent != NULL) {
        parent->entries = realloc(parent->entries, sizeof(struct inode*) * (parent->num_entries+1));
        parent->entries[parent->num_entries] = (uintptr_t) new_file;
        parent->num_entries++;
    }

    return new_file;
}

struct inode* create_dir(struct inode* parent, char* name) {
    // Sjekke om navn er unikt
    if (find_inode_by_name(parent, name) != NULL) {
        perror("Already exists");
        return NULL;
    }

    // Allokere minne til ny node
    struct inode* new_dir = malloc(sizeof(struct inode));
    if (new_dir == NULL) {
        perror("Memory allocation error");
        return NULL;
    }

    // Allokerer minne for navn stringen og kopiere innholdet av navn inn i den
    new_dir->name = malloc(strlen(name) + 1);
    if (new_dir->name == NULL) {
        perror("Memory allocation error");
        free(new_dir);
        return NULL;
    }
    strcpy(new_dir->name, name);

    // Setter verdier til variabler
    new_dir->id = globalIdCount++;
    new_dir->is_directory = 1;
    new_dir->is_readonly = 0;
    new_dir->filesize = 0;
    new_dir->num_entries = 0;
    new_dir->entries = NULL;

    // Legger den nye noden til parent's entries array
    if (parent != NULL) {
        parent->entries = realloc(parent->entries, (parent->num_entries + 1) * sizeof(struct inode*));
        if (parent->entries == NULL) {
            perror("Memory allocation error");
            free(new_dir->name);
            free(new_dir);
            return NULL;
        }
        parent->entries[parent->num_entries] = (uintptr_t) new_dir;
        parent->num_entries++;
    }

    return new_dir;
}


struct inode* find_inode_by_name(struct inode* parent, char* name) {
    //sjekke om det er en tom mappe eller ikke
    if (parent == NULL) {
        perror("Parent doesn't have any entries");
        return NULL;
    }

    for (int i = 0; i < parent->num_entries; i++) {
        //sjekke hver entry i parent om noen har samme navn
        struct inode* entry = (struct inode*) parent->entries[i];
        if (!strcmp(entry->name, name)) {
            return entry; 
        }
    }

    return NULL;
}

struct inode* load_inodes() {
    FILE *file = fopen("master_file_table", "rb");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    struct inode** myInodes = NULL;
    struct inode** myInodes_new = NULL;
    struct inode* inode;
    int arraySize = 0;
    
    while (1) { 
        inode = malloc(sizeof(struct inode));
        
        if(!(fread((char*) &inode->id, sizeof(char), 4, file))) {
            free(inode);
            break;
        }

        int lengde;
        fread((char*) &lengde, sizeof(int), 1, file);
        inode->name = malloc(lengde + 1);
        fread(inode->name, sizeof(char), lengde, file);
        inode->name[lengde] = '\0';
        fread(&inode->is_directory, sizeof(char), 1, file);
        fread(&inode->is_readonly, sizeof(char), 1, file);
        fread((char*) &inode->filesize, sizeof(int), 1, file);
        fread((char*) &inode->num_entries, sizeof(int), 1, file);

        inode->entries = malloc(sizeof(uintptr_t) * inode->num_entries);
        fread(inode->entries, sizeof(uintptr_t), inode->num_entries, file);

        arraySize += sizeof(struct inode*);
        myInodes_new = realloc(myInodes, arraySize * sizeof(struct inode*));
        if (!myInodes_new) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        myInodes = myInodes_new;
        myInodes[inode->id] = inode;
    }    

    struct inode* rotnode = myInodes[0];

    // Setter pekere riktig
    for (long unsigned int i = 0; i < arraySize / sizeof(struct inode*); i++) {        
        if (myInodes[i]->is_directory) {
            for (int j = 0; j < myInodes[i]->num_entries; j++) {
                myInodes[i]->entries[j] = (uintptr_t) myInodes[myInodes[i]->entries[j]];
            }
        }
    }

    free(myInodes);
    fclose(file);
    return rotnode;
}

void fs_shutdown(struct inode* inode) {
    if (inode == NULL) {
        return;
    }

    if(!inode->is_directory){
        for(int i = 0; i < inode->num_entries; i++){
            free_block(inode->entries[i]);
        }

        free(inode->entries);
        free(inode->name);
        free(inode);
        return;
    } else {
        for(int i = 0; i < inode->num_entries; i++){
            fs_shutdown((struct inode *) inode->entries[i]);
        }

        free(inode->name);
        free(inode->entries);
        free(inode);
        inode = NULL;
    }
}

/* This static variable is used to change the indentation while debug_fs
 * is walking through the tree of inodes and prints information.
 */
static int indent = 0;

void debug_fs( struct inode* node )
{
    if( node == NULL ) return;
    for( int i=0; i<indent; i++ )
        printf("  ");
    if( node->is_directory )
    {
        printf("%s (id %d)\n", node->name, node->id );
        indent++;
        for( int i=0; i<node->num_entries; i++ )
        {
            struct inode* child = (struct inode*)node->entries[i];
            debug_fs( child );
        }
        indent--;
    }
    else
    {
        printf("%s (id %d size %db blocks ", node->name, node->id, node->filesize );
        for( int i=0; i<node->num_entries; i++ )
        {
            printf("%d ", (int)node->entries[i]);
        }
        printf(")\n");
    }
}
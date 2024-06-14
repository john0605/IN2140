/*
 * This file implements two functions that read XML and binary information from a buffer,
 * respectively, and return pointers to Record or NULL.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "recordFromFormat.h"
#include <string.h>
#include <stdlib.h>

#define GRADE_BACHELOR 0
#define GRADE_MASTER 1
#define GRADE_PHD 2

Record* XMLtoRecord( char* buffer, int bufSize, int* bytesread )
{
    /*sjekker om jeg får riktig input her. 
    printf("xml to record her");
    printf("THIS ID BUFFER %s", buffer);*/
    //printf("strstr: %s\n\n", test);
    //char *test = strstr(tmpBuffer, "source=");
    //setSource(record, test);
    //printf("inn i xml to record%s", buffer);

    Record *record = newRecord();
    clearRecord(record);

    //SOURCE
    char source;
    //tmp for å ikke ødlegge buffere
    char* sourceBuffer = malloc(strlen(buffer) + 1);
    strcpy(sourceBuffer, buffer);
    sourceBuffer = strstr(sourceBuffer, "source=");
    //ekstraher source
    memcpy(&source, sourceBuffer+8, 1);
    //printf("sourceTest: %c\n\n", source);
    setSource(record, source);
    //fflush(stdout);

    //DEST
    char dest; 
    char* destBuffer = malloc(strlen(buffer) + 1);
    strcpy(destBuffer, buffer);
    destBuffer = strstr(destBuffer, "dest=");
    //ekstraher dest
    //teller det opp til det som er etter dest=
    //gjort det på denne maaten utover funksjonen
    memcpy(&dest, destBuffer+6, 1);
    //printf("destTest: %c\n\n", dest);
    setDest(record, dest);
    //fflush(stdout);

    //Username
    char* unBuffer = malloc(strlen(buffer)+1);
    strcpy(unBuffer, buffer);
    unBuffer = strstr(unBuffer, "username=");
    if(unBuffer != NULL){
        unBuffer = strtok(unBuffer, "\"");
        unBuffer = strtok(NULL, "\"");
        //printf("unTest: %s\n\n", unBuffer);
        setUsername(record, unBuffer);
        //fflush(stdout);
    }

    //ID
    char* idBuffer = malloc(strlen(buffer) + 1);
    strcpy(idBuffer, buffer);
    idBuffer = strstr(idBuffer, "id=");
    //ekstraherer id
    if(idBuffer != NULL){
        idBuffer = strtok(idBuffer, "\"");
        idBuffer = strtok(NULL, "\"");
        uint32_t castId = atoi(idBuffer); 
        setId(record, castId);
        //printf("idTest: %d\n\n", castId);
        //fflush(stdout);
    }

    //Group
    char* groupBuffer = malloc(strlen(buffer) + 1);
    strcpy(groupBuffer, buffer);
    groupBuffer = strstr(groupBuffer, "group=");
    //ekstraherer group
    if(groupBuffer != NULL){
        groupBuffer = strtok(groupBuffer, "\"");
        groupBuffer = strtok(NULL, "\"");
        uint32_t castGroup = atoi(groupBuffer); 
        setGroup(record, castGroup);
        //printf("groupTest: %d\n\n", castGroup);
        //fflush(stdout);
    }

    //Course
    char* courseBuffer = malloc(strlen(buffer) + 1);
    strcpy(courseBuffer, buffer);
    courseBuffer = strstr(courseBuffer, "course=");
    //ekstraherer group
    if(courseBuffer != NULL){
        courseBuffer = strtok(courseBuffer, "\"");
        courseBuffer = strtok(NULL, "\"");
        while(1){
            //starter med 1 her fordi at det er forste aars fag. 
            int castCourse = atoi(strchr(courseBuffer, '1'));
            courseBuffer = strtok(NULL, "\"");
            //printf("courseTest: %d\n\n", castCourse);

            //Gaar gjennom alle de emne og setter dem til tallet 
            //record.c onsker aa ha. 
            if(castCourse == 1000){
                setCourse(record, 1);
            }else if(castCourse == 1010){
                setCourse(record, 2);
            }else if(castCourse == 1020){
                setCourse(record, 4);
            }else if(castCourse == 1030){
                setCourse(record, 8);
            }else if(castCourse == 1050){
                setCourse(record, 16);
            }else if(castCourse == 1060){
                setCourse(record, 32);
            }else if(castCourse == 1080){
                setCourse(record, 64);
            }else if(castCourse == 1140){
                setCourse(record, 128);
            }else if(castCourse == 1150){
                setCourse(record, 256);
            }else if(castCourse == 1900){
                setCourse(record, 512);
            }else if(castCourse == 1910){
                setCourse(record, 1024);
            }

            if (strstr(courseBuffer, "</courses>") != NULL){
                break;
            }
        courseBuffer = strtok(NULL, "\"");
        //printf("%c", courseBuffer);
        //fflush(stdout);
        }
    }
    
    //Semester
    char* semesterBuffer = malloc(strlen(buffer) + 1);
    strcpy(semesterBuffer, buffer);
    semesterBuffer = strstr(semesterBuffer, "semester=");
    //ekstraherer id
    if(semesterBuffer != NULL){
        semesterBuffer = strtok(semesterBuffer, "\"");
        semesterBuffer = strtok(NULL, "\"");
        uint8_t castSemester = atoi(semesterBuffer); 
        setSemester(record, castSemester);
        //printf("semesterTest: %d\n\n", castSemester);
        //fflush(stdout);
    }

    //Grade
    char* gradeBuffer = malloc(strlen(buffer)+1);
    strcpy(gradeBuffer, buffer);
    gradeBuffer = strstr(gradeBuffer, "grade=");
    if(gradeBuffer != NULL){
        gradeBuffer = strtok(gradeBuffer, "\"");
        gradeBuffer = strtok(NULL, "\"");

        if(strcmp(gradeBuffer, "Bachelor") == 0){
            Grade gradeVar = Grade_Bachelor;
            setGrade(record, gradeVar);
        }else if(strcmp(gradeBuffer, "Master") == 0){
            Grade gradeVar = Grade_Master;
            setGrade(record, gradeVar);
            //printf("masterTest: %s\n\n", gradeBuffer);
        }else if(strcmp(gradeBuffer, "PhD") == 0){
            Grade gradeVar = Grade_PhD;
            setGrade(record, gradeVar);
        }
    }
    return record;
}


Record* BinaryToRecord( char* buffer, int bufSize, int* bytesread )
{   
    // lager en ny record. 
    Record* record = newRecord();
    clearRecord(record);

    // leser typen i denne blocken
    uint8_t flagene;
    memcpy(&flagene, buffer, sizeof(uint8_t));
    buffer += sizeof(uint8_t);
    *bytesread += sizeof(uint8_t);

    // sjekker flaggene for source
    if (flagene & FLAG_SRC) {
        char source;
        memcpy(&source, buffer, sizeof(char));
        buffer += sizeof(char);
        *bytesread += sizeof(char);
        //printer ut for å sjekke om jeg faktisk leser ut riktig her -- videre i dok. 
        //printf("\nDette er source: %c", source);
        setSource(record, source);
    }

    // sjekker flaggene for dest
    if (flagene & FLAG_DST) {
        char dest;
        memcpy(&dest, buffer, sizeof(char));
        buffer += sizeof(char);
        *bytesread += sizeof(char);
        //gjor det samme her for aa se om jeg faar ut riktig dest
        //printf("\nDette er dest: %c", dest);
        setDest(record, dest);
    }
    
    //sjekker flaggene for username
    /*denne blocken med koder fungerer ikke og vil kunne gi
    segmentation error, denne metoden vil ellers fungere dersom denne
    delen blir kommentert bort*/

    // /*
    if (flagene & FLAG_USERNAME) {
        uint32_t nameLength;
        memcpy(&nameLength, buffer, sizeof(uint32_t));
        buffer += sizeof(uint32_t);
        *bytesread += sizeof(uint32_t);

        char* username = malloc(nameLength + 1);
        if (username == NULL) {
            fprintf(stderr, "Failed to allocate memory for username\n");
        } else {
            memcpy(username, buffer, nameLength);
            username[nameLength] = '\0';
            buffer += nameLength;
            *bytesread += nameLength;

            setUsername(record, username);
            free(username);
        }
    }
    // */

    // sjekker flaggene for group
    if (flagene & FLAG_GROUP) {
        uint32_t group;
        memcpy(&group, buffer, sizeof(uint32_t));
        buffer += sizeof(uint32_t);
        *bytesread += sizeof(uint32_t);
        setGroup(record, group);
    }

    // sjekker flaggene for semester
    if (flagene & FLAG_SEMESTER) {
        uint8_t semester;
        memcpy(&semester, buffer, sizeof(uint8_t));
        buffer += sizeof(uint8_t);
        *bytesread += sizeof(uint8_t);
        setSemester(record, semester);
    }

    // sjekker flaggene for grade
    if (flagene & FLAG_GRADE) {
        uint8_t grade;
        memcpy(&grade, buffer, sizeof(uint8_t));
        buffer += sizeof(uint8_t);
        *bytesread += sizeof(uint8_t);
        setGrade(record, grade);
    }

    // sjekker flaggene for id
    if (flagene & FLAG_ID) {
        uint32_t id;
        memcpy(&id, buffer, sizeof(uint32_t));
        buffer += sizeof(uint32_t);
        *bytesread += sizeof(uint32_t);
        setId(record, id);
    }

    // sjekker flaggene for courses
    if (flagene & FLAG_COURSES) {
        uint16_t courses;
        memcpy(&courses, buffer, sizeof(uint16_t));
        buffer += sizeof(uint16_t);
        *bytesread += sizeof(uint16_t);
        setCourse(record, courses);
    }

    //fflush(stdout);
    return record;
}
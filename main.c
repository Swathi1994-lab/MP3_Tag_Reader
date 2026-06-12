/*Name:D Swathi
Roll No:25040B_051
Date:23/03/2026
Project Name:MP3_Tag_Reader
Description:
MP3 tag reader is a software which will read and display MP3(ID3) tag information from MP3 files.
ID3 is a metadata container most often used in conjuction with the MP3 audio file format.
It allows information such as the title ,artist,album,track number and other information about the file to be stored in the file itself.

View:In view function it will read the information and display tag information such as the title ,artist,album name,year and Genre

Edit:Edit mode is used to modify or update the metadata of an MP3 file. To change existing tag values such as title,album,artist,
composer,year and Genre.

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//convert 4 byte big endian array to integer
int bigEndian_to_int(unsigned char *size)
{
      //combine bytes into integer
     return (size[0]<<24)|(size[1]<<16)|(size[2]<<8)|size[3];
}
//convert integer to 4 byte big endian format
void int_to_bigEndian(int size,unsigned char * bytes)
{
      bytes[0] = (size>>24) & 0xFF;  //store MSB
      bytes[1] = (size>>16) & 0xFF;  //store next byte
      bytes[2] = (size>>8) & 0xFF;   //store next byte
      bytes[3] =  size & 0xFF;       //store LSB
}
//function for view mp3 tag reader
int view_tag(char *filename)
{
      //Declare file pointer
      FILE *fp;
      //open file in binary read mode
      if((fp = fopen(filename,"rb"))==NULL)
      {
            fprintf(stderr,"Error opening file\n");
            return 1;
       }
       char id[3];
       //read first 3 bytes
       fread(id,1,3,fp);
       //check if file ID3 contains
       if(strncmp(id,"ID3",3)!=0)
       {
            printf("ID3 tag is not found\n");
            fclose(fp);
            return 1;
       }
      unsigned char version[2];
       //read version 2 bytes
       fread(version, 1, 2, fp);
       //check the version
      if(version[0] != 3)
      {
         printf("ID3v2.3 not found\n");
         fclose(fp);
         return 1;
      }
       //skip next 5 bytes
       fseek(fp,5,SEEK_CUR);
       //start reading frames
       printf("----------------------------------------------------------\n");
       printf("MP3 Tag Reader and Editor for ID3v2\n");
       printf("----------------------------------------------------------\n");
       int count = 0;
       //Read first 6 frames
       while(count<6)
       {
            char frame_id[5];
            //read 4 bytes for frame id
            if((fread(frame_id,1,4,fp))!=4)
            break;
            frame_id[4] = '\0';
            //stop if invalid frame id
            if(frame_id[0]==0)
            break;
            
             unsigned char size_bytes[4];
             //read frame size
             fread(size_bytes,1,4,fp);
             //convert size
           unsigned int size = bigEndian_to_int(size_bytes);
            //skip 2 flag bytes
            fseek(fp,2,SEEK_CUR);
            //allocate memory for frame data
            char* frame_data = malloc(size+1);
            //read frame data
            fread(frame_data,1,size,fp);
            frame_data[size] = '\0';
           
            //print only required frames
            //print title
            if(strcmp(frame_id,"TIT2")==0)
            {
            printf("Title : %s\n",frame_data+1);
            }
            //print artist
           else if(strcmp(frame_id,"TPE1")==0)
           {
            printf("Artist : ");
             for(int i=0;i<size;i++)
            {
                 printf("%c",frame_data[i]);
            }
            printf("\n");
           }
           //print album
            else if(strcmp(frame_id,"TALB")==0)
            {
            printf("Album : ");
            for(int i=0;i<size;i++)
            {
                 printf("%c",frame_data[i]);
            }
            printf("\n");
            }
            //print year 
            else if(strcmp(frame_id,"TYER")==0)
            {
            printf("Year : ");
            for(int i=0;i<size;i++)
            {
                 printf("%c",frame_data[i]);
            }
            printf("\n");
            } 
            //print Genre    
            else if(strcmp(frame_id,"TCON")==0)
            {
            printf("Content Type : ");
            for(int i=0;i<size;i++)
            {
                 printf("%c",frame_data[i]);
            }
            printf("\n");
            }
            //print composer
            else if(strcmp(frame_id,"COMM")==0)
            {
            printf("Composer : ");
            for(int i=0;i<size;i++)
            {
                 printf("%c",frame_data[i]);
            }
            printf("\n");
            }
            //Increment frame counter
            count++;
            //free allocated memory
            free(frame_data);
      }
             printf("----------------------------------------------------------\n");
             fclose(fp); //close file
             return 0;
}
//function to edit specific ID3 frame
int edit_tag(char *option,char *new_val,char *filename)
{
            //source file pointer
            FILE *src;
            //destination file pointer
            FILE *dest;
            //open source file in binary read mode
            if((src=fopen(filename,"rb"))==NULL)
            {
                  fprintf(stderr,"file not found\n");
                  return 1;
            }
            //open destination file in binary read mode
            if((dest=fopen("temp.mp3","wb"))==NULL)
            {
                  fprintf(stderr,"File not found\n");
                  return 1;
            }
              
            char header[10];
            fread(header,1,10,src);  //read header
            fwrite(header,1,10,dest); //write header

           char frame_to_edit[5];
            //choose option for edit frame id
            if(strcmp(option,"-t")==0)
              strcpy(frame_to_edit,"TIT2");
            else if(strcmp(option,"-a")==0)
              strcpy(frame_to_edit,"TPE1");
            else if(strcmp(option,"-A")==0)
              strcpy(frame_to_edit,"TALB");
            else if(strcmp(option,"-y")==0)
              strcpy(frame_to_edit,"TYER");
            else if(strcmp(option,"-g")==0)
              strcpy(frame_to_edit,"TCON");
            else if(strcmp(option,"-c")==0)
              strcpy(frame_to_edit,"COMM");
            else 
            {
                  printf("Invalid edit option\n");
                  return 1;
            }
            
            while(1)
            {
                  char frame_id[5];
                  if((fread(frame_id,1,4,src))!=4)
                  break;
                  if(frame_id[0]==0)
                    break;
                  frame_id[4] = '\0';
                  unsigned char size_bytes[4];
                  fread(size_bytes,1,4,src);
                  int old_size = bigEndian_to_int(size_bytes);

                  char flags[2];
                  fread(flags,1,2,src);
                   //if frame id match edit tag
                  if(strcmp(frame_id,frame_to_edit)==0)
                  {
                        //write frame id
                        fwrite(frame_id,1,4,dest);
                        
                        int new_size = strlen(new_val)+1;
                        unsigned char new_size_bytes[4];
                        //convert new size to big endian
                        int_to_bigEndian(new_size,new_size_bytes);
                         //write new size bytes
                        fwrite(new_size_bytes,1,4,dest);
                        //write flags
                        fwrite(flags,1,2,dest);
                        //write encoding byte
                        char encoding = 0;
                        fwrite(&encoding,1,1,dest);
                        //write new data
                        fwrite(new_val,1,strlen(new_val),dest);
                        //skip old content
                        fseek(src,old_size,SEEK_CUR);

                  }
                  else
                  {
                        //copy frame as it is
                        fwrite(frame_id,1,4,dest);
                        fwrite(size_bytes,1,4,dest);
                        fwrite(flags,1,2,dest);

                        char *buffer = malloc(old_size);
                        fread(buffer,1,old_size,src);
                        fwrite(buffer,1,old_size,dest);
                        free(buffer);
                  }
            }
          
            fclose(src);  //close source file
            fclose(dest);  //close destination file
            //Delete old file
            remove(filename);
            //rename backup to original
            rename("temp.mp3",filename);
            printf("Tag Updated successfully\n");
           return 0;
            
}
int main(int argc,char* argv[])
{
      if(argc < 3)
{
    printf("Enter valid no.of arguments:./a.out -v or -e file.mp3\n");
    return 1;
}
      //check the option -v
      if(strcmp(argv[1],"-v")==0)
      {
         if(argc != 3)
         {
            printf("Invalid arguments\n");
            return 1;
       }
        //check mp3 extension
       if(strstr(argv[2],".mp3")==NULL)
       {
            printf("File name should be .mp3\n");
            return 1;
       }
      view_tag(argv[2]);
     }
     else if(strcmp(argv[1],"-e")==0)
     {
      if(argc!=5)
      {
         printf("Invalid arguments\n");
         return 1;
      }
      //check mp3 extension
       if(strstr(argv[4],".mp3")==NULL)
       {
            printf("File name should be .mp3\n");
            return 1;
       }
      edit_tag(argv[2],argv[3],argv[4]);
     }
     else
     {
      printf("Invalid option\n");
      return 1;

     }

     return 0;  
           
}


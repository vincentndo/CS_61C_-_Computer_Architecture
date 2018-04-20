#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - Here are some of the helper functions from util.h:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the project spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);

  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

  return 0;
}



/* beargit add <filename>
 *
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR:  File <filename> has already been added.
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR:  File %s has already been added.\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the project spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index","r");
    
  char line[FILENAME_SIZE];
  int count=0;

  printf("Tracked files:\n\n");
  while (fgets(line,sizeof(line),findex))  
    {
      strtok(line,"\n");
      printf("%s\n",line);
      count++;
    }
  fclose(findex);
  printf("\nThere are %d files total.\n",count);
  return 0;
}

/* beargit rm <filename>
 *
 * See "Step 2" in the project spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  FILE* fnewindex = fopen(".beargit/.newindex","w");

  char line[FILENAME_SIZE];
  int filetracked=0;

  while (fgets(line,sizeof(line),findex))
    {
      strtok(line,"\n");
      if (strcmp(line, filename) != 0)
	{	  
	  fprintf(fnewindex,"%s\n",line); 	  
	}
      else
	{
	  filetracked++;
	}
    }
  fclose(findex);
  fclose(fnewindex);

  if (!filetracked)
    {
      fs_rm(".beargit/.newindex");
      fprintf(stderr, "ERROR:  File %s not tracked.\n", filename);
      return 1;
    }
  else
    {
      fs_mv(".beargit/.newindex",".beargit/.index");
      return 0;
    }
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the project spec.
 *
 */

const char* go_bears = "THIS IS BEAR TERRITORY!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  int i = 0;
  /*while (msg[i] != '\0')
     {
        if (msg[i] != go_bears[i])
  	{
  	  return 0;
  	}
        else
  	{
  	  i++;
  	}
     }
     return 1;*/
  while (*(msg + i) != '\0') {
    int k = 0;
    if (*(msg + i) == *go_bears)
      {
	int j = i;
	while (*(msg+j)!='\0' && *(go_bears+k)!='\0' && *(msg+j)==*(go_bears+k))
	  {
	    if (k == 22)
	      {
		return 1;
	      }
	    k++;
	    j++;
	  }
      }
    i++;
  }
  return 0;
}

/* Use next_commit_id to fill in the rest of the commit ID.
 *
 * Hints:
 * You will need a destination string buffer to hold your next_commit_id, before you copy it back to commit_id
 * You will need to use a function we have provided for you.
 */

char dst_buf[COMMIT_ID_SIZE];

void next_commit_id(char* commit_id) {
     /* COMPLETE THE REST */
  
  /*char id_branch[COMMIT_ID_SIZE+BRANCHNAME_SIZE];
  char id_buffer[COMMIT_ID_SIZE];
  
  read_string_from_file(".beargit/.current_branch",id_branch,BRANCHNAME_SIZE);
  strcat(id_branch, commit_id);
  cryptohash(id_branch,id_buffer);
  strcpy(commit_id,id_buffer);*/

  char branch_commit[BRANCHNAME_SIZE + COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.current_branch", branch_commit, BRANCHNAME_SIZE);
  strcat(branch_commit, commit_id);
  cryptohash(branch_commit, dst_buf);
}

int beargit_commit(const char* msg) {
  /*if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR:  Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char currbranch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch",currbranch, BRANCHNAME_SIZE);
  if (currbranch[0] == '\0')
    {
      fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit.");
      return 1;
    }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);*/

  /* COMPLETE THE REST */

  // Make new directory .beaargit/commit_id and copy files into /commit_id
  /*char newdir[BRANCHNAME_SIZE];
  char newindex[FILENAME_SIZE];
  char newprev[FILENAME_SIZE];
  sprintf(newdir,".beargit/%s",commit_id);
  fs_mkdir(newdir);
  sprintf(newindex,"%s/.index",newdir);
  sprintf(newprev,"%s/.prev",newdir);
  fs_cp(".beargit/.index",newindex);
  fs_cp(".beargit/.prev",newprev);

  FILE* findex = fopen(".beargit/.index","r");
  char line[FILENAME_SIZE];
  char file_src[FILENAME_SIZE];
  char file_dst[FILENAME_SIZE];
  
  while (fgets(line,sizeof(line),findex))
    {
      strtok(line,"\n");
      sprintf(file_src,".beargit/%s",line);
      sprintf(file_dst,"%s/%s",newdir,line);
      fs_cp(file_src,file_dst);
    }
  fclose(findex);

  // Store message
  char msgfname[FILENAME_SIZE];
  sprintf(msgfname,"%s/.msg",newdir);
  write_string_to_file(msgfname,msg);

  // Write new ID into .prev  
  //  FILE* fprev = fopen(".beargit/.prev","r");
  //  FILE* fnewprev = fopen(".beargit/.newprev","w");
  //  char lineprev[COMMIT_ID_SIZE+10];

  //  fprintf(fnewprev,"%s\n",commit_id);
  //  while (fgets(lineprev,sizeof(lineprev),fprev))
  //    {
  //      strtok(lineprev,"\n");
  //      fprintf(fnewprev,"%s\n",lineprev);
  //    }
  //  fclose(".beargit/.prev");
  //  fclose(".beargit/.newprev");

  //  fs_mv(".beargit/.newprev",".beargit/prev");
  write_string_to_file(".beargit/.prev",commit_id);  

  return 0;*/

  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR:  Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  /*code not given start*/
  char branch_name[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", branch_name, BRANCHNAME_SIZE);
  if (branch_name[0] == '\0') {
    fprintf(stderr, "ERROR:  Need to be on HEAD of a branch to commit.\n");
    return 1;
  }
  /*code not given end*/

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  char new_dir[10 + COMMIT_ID_SIZE] = ".beargit/"; 
  strcat(new_dir, dst_buf);  
  fs_mkdir(new_dir);
  strcat(new_dir, "/");  /*new_dir is ".beargit/<newid>/" */

  char new_dst[9 + COMMIT_ID_SIZE + FILENAME_SIZE];
  strcpy(new_dst, new_dir);

  FILE* findex = fopen(".beargit/.index", "r");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    strcat(new_dst, line);
    fs_cp(line, new_dst);
    strcpy(new_dst, new_dir);
  }

  fclose(findex);

  strcat(new_dst, ".index");
  fs_cp(".beargit/.index", new_dst);
  strcpy(new_dst, new_dir);

  strcat(new_dst, ".prev");
  fs_cp(".beargit/.prev", new_dst);
  strcpy(new_dst, new_dir);

  strcat(new_dst, ".msg");
  write_string_to_file(new_dst, msg);
  write_string_to_file(".beargit/.prev", dst_buf);

  return 0;
}

/* beargit log
 *
 * See "Step 4" in the project spec.
 *
 */

int beargit_log(int limit) {
  /* COMPLETE THE REST */
  
  int count = 0;
  char commit_id[COMMIT_ID_SIZE+10];
  char no_commit[COMMIT_ID_SIZE+10]="0000000000000000000000000000000000000000";
  char newdir[BRANCHNAME_SIZE];
  char newprev[FILENAME_SIZE];
  char msgfname[FILENAME_SIZE];
  char msg[MSG_SIZE];

  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  if (strcmp(commit_id,no_commit)==0)
    {
      fprintf(stderr, "ERROR:  There are no commits.\n");
      return 1;
    }
  while (strcmp(commit_id, no_commit)!=0 && (count<=limit))
    {
      sprintf(newdir,".beargit/%s",commit_id);
      printf("commit %s\n",commit_id);
      sprintf(msgfname, "%s/.msg",newdir);
      read_string_from_file(msgfname, msg, MSG_SIZE);
      printf("   %s\n\n",msg);

      sprintf(newprev,"%s/.prev",newdir);
      read_string_from_file(newprev, commit_id, COMMIT_ID_SIZE);
      count++;
     }
  return 0;
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 5" in the project spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */

  FILE* fbranches = fopen(".beargit/.branches","r");
  char line[BRANCHNAME_SIZE];
  char currbranch[BRANCHNAME_SIZE];

  read_string_from_file(".beargit/.current_branch", currbranch, BRANCHNAME_SIZE);
  while (fgets(line,sizeof(line),fbranches))
    {
      strtok(line,"\n");
      if (!strcmp(line,currbranch))
	{
	  printf("*  %s\n",line);
	}
      else
	{
	  printf("   %s\n",line);
	}
    }
  fclose(fbranches);

  return 0;
}

/* beargit checkout
 *
 * See "Step 6" in the project spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */

  char no_commit[COMMIT_ID_SIZE+10]="0000000000000000000000000000000000000000";

  FILE* findex = fopen(".beargit/.index","r");
  char line[FILENAME_SIZE];
  char fname[FILENAME_SIZE+9];

  if (strcmp(commit_id,no_commit)==0)
    {
      // Delete files in current dir
      while (fgets(line,sizeof(line),findex))
	{
	  strtok(line,"\n");
	  sprintf(fname,".beargit/%s",line);
	  fs_rm(fname);
	}
      fclose(findex);
      // Empty index
      write_string_to_file(".beargit/.index", "");
      // Write 00 commit ID into .prev
      write_string_to_file(".beargit/.prev", commit_id);
      return 0;
    }
        
  // Delete files in current dir
  while (fgets(line,sizeof(line),findex))
    {
      strtok(line,"\n");
      sprintf(fname,".beargit/%s",line);
      fs_rm(fname);
    }
  fclose(findex);

  char cindex[FILENAME_SIZE+50];
  
  //Copy index file from commit dir to current dir
  sprintf(cindex,".beargit/%s/.index",commit_id);
  fs_cp(cindex,".beargit/.index");

  FILE* fcindex = fopen(".beargit/.index","r");
  char cline[FILENAME_SIZE];
  char fcname[FILENAME_SIZE+50];
  
  sprintf(fcname,".beargit/%s/",commit_id);
  while (fgets(cline,sizeof(cline),fcindex))
    {
      strtok(cline,"\n");
      strcat(fcname,cline);
      sprintf(fname,".beargit/%s",cline);
      fs_cp(fcname, fname);
    }
  fclose(fcindex);

  write_string_to_file(".beargit/.prev", commit_id);
  
  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */

  /*char prev_id[COMMIT_IS_SIZE];
  char no_commit[COMMIT_ID_SIZE]="0000000000000000000000000000000000000000";
  char newprev[FILENAME_SIZE];

  read_string_from_file(".beargit/.prev", prev_id, COMMIT_ID_SIZE);

  if (strcmp(commit_id,prev_id)==0)
    {
      return 1;
    }
  else
    {
      sprintf(newprev,".beargit/%s/.prev",prev_id);
      read_string_from_file(newprev, prev_id, COMMIT_ID_SIZE);
      while (strcmp(commit_id, no_commit)!=0)
	{
	  if (strcmp(commit_id,prev_id)==0)
	    {
	      return 1;
	    }
	  else
	    {
	      sprintf(newprev,".beargit/%s/.prev",prev_id);
	      read_string_from_file(newprev, prev_id, COMMIT_ID_SIZE);
	    }
	}
   }
   return 0;*/
	
  char commit_dir[BRANCHNAME_SIZE] = ".beargit/";
  strcat(commit_dir, commit_id);
  if (fs_check_dir_exists(commit_dir))
    {
      return 1;
    }
  return 0;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }

  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    //if (!fs_check_dir_exists(commit_dir)) {
    // fprintf(stderr, "ERROR:  Commit %s does not exist.\n", arg);
      //  return 1;
      //}

    // Set the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "");

    return checkout_commit(arg);
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(branch_name) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR:  A branch named %s already exists.\n", branch_name);
    return 1;
  } else if (!branch_exists && !new_branch) {
    fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", branch_name);
    return 1;
  }

  // File for the branch we are changing into.
  char branch_file[BRANCHNAME_SIZE+20] = ".beargit/.branch_";
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file);
  }

  write_string_to_file(".beargit/.current_branch", branch_name);

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}

/* beargit reset
 *
 * See "Step 7" in the project spec.
 *
 */

int beargit_reset(const char* commit_id, const char* filename) {
  if (!is_it_a_commit_id(commit_id)) {
      fprintf(stderr, "ERROR:  Commit %s does not exist.\n", commit_id);
      return 1;
  }

  // Check if the file is in the commit directory
  /* COMPLETE THIS PART */
  char cindex[FILENAME_SIZE+50];

  sprintf(cindex,".beargit/%s/.index",commit_id);
  FILE* fcindex = fopen(cindex,"r");
  char cline[FILENAME_SIZE];
  int indicator_1=0;
  
  while (fgets(cline,sizeof(cline),fcindex))
    {
      strtok(cline,"\n");
      if (strcmp(filename, cline)==0)
	{
	  indicator_1++;	  
	}
    }
  fclose(fcindex);

  if (indicator_1==0)
    {
      fprintf(stderr, "ERROR:  %s is not in the index of commit %s.",filename, commit_id);
      return 1;
    }

  // Copy the file to the current working directory
  /* COMPLETE THIS PART */
  char fname[FILENAME_SIZE+9];
  char fcname[FILENAME_SIZE+50];
  
  sprintf(fname,".beargit/%s",filename);
  sprintf(fcname,".beargit/%s/%s",commit_id,filename);
  fs_cp(fcname,fname);

  // Add the file if it wasn't already there
  /* COMPLETE THIS PART */
  FILE* findex = fopen(".beargit/.index","r");
  char line[FILENAME_SIZE];
  int indicator_2=0;
  
  while (fgets(line,sizeof(line),findex))
    {
      strtok(line,"\n");
      if (strcmp(filename, line)==0)
	{
	  return 0;	  
	}     
    }
  fclose(findex);

  findex = fopen(".beargit/.index","a");
  fprintf(findex, "%s\n", filename);
  fclose(findex);

  return 0;
}

/* beargit merge
 *
 * See "Step 8" in the project spec.
 *
 */

int beargit_merge(const char* arg) {
  // Get the commit_id or throw an error
  char commit_id[COMMIT_ID_SIZE];
  if (!is_it_a_commit_id(arg)) {
      if (get_branch_number(arg) == -1) {
            fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
            return 1;
      }
      char branch_file[FILENAME_SIZE];
      snprintf(branch_file, FILENAME_SIZE, ".beargit/.branch_%s", arg);
      read_string_from_file(branch_file, commit_id, COMMIT_ID_SIZE);
  } else {
      snprintf(commit_id, COMMIT_ID_SIZE, "%s", arg);
  }

  // Iterate through each line of the commit_id index and determine how you
  // should copy the index file over
   /* COMPLETE THE REST */
  char cindex[FILENAME_SIZE+50];
  sprintf(cindex,".beargit/%s/.index",commit_id);
  FILE* fcindex = fopen(cindex,"r");
  char cline[FILENAME_SIZE];
  char fcname[FILENAME_SIZE+50];
  char fcname_new[FILENAME_SIZE+50];

  char line[FILENAME_SIZE+10];
  char fname[FILENAME_SIZE+10];
  int indicator;
  
  while (fgets(cline,sizeof(cline),fcindex))
    {
      strtok(cline,"\n");
      sprintf(fcname,".beargit/%s/%s",commit_id,cline);
      indicator = 0;
      FILE* findex = fopen(".beargit/.index","r");
      while (fgets(line,sizeof(line),findex))
	{
	  strtok(line,"\n");	  
	  if (strcmp(line,cline)==0)
	    {
	      indicator++;
	      sprintf(fname,".beargit/%s.%s",cline,commit_id);
	      fs_cp(fcname,fname);
       	      printf("%s conflicted copy created\n",line);
	    }
	}
      fclose(findex);
      if (indicator==0)
	{
	  sprintf(fcname_new,".beargit/%s",cline);
	  fs_cp(fcname,fcname_new);
	  findex = fopen(".beargit/.index","a");
	  fprintf(findex, "%s\n", cline);
	  fclose(findex);
      	  printf("%s added\n",cline);
	}
    }
  fclose(fcindex);

  return 0;
}

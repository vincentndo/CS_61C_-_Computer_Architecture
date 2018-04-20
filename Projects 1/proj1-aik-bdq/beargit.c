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
  FILE* findex = fopen(".beargit/.index", "r");

  int counter = 0;
  char line[FILENAME_SIZE];
  fprintf(stdout, "Tracked files:\n\n");
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    fprintf(stdout, "%s\n", line);
    counter++;
  }

  fclose(findex);

  fprintf(stdout, "\nThere are %d files total.\n", counter);
  return 0;
}

/* beargit rm <filename>
 *
 * See "Step 2" in the project spec.
 *
 */

int beargit_rm(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  int filetracked = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename)) {
      fprintf(fnewindex, "%s\n", line);
    } else {
      filetracked++;
    }
  }

  fclose(findex);
  fclose(fnewindex);

  if (!filetracked) {
    fs_rm(".beargit/.newindex");
    fprintf(stderr, "ERROR:  File %s not tracked.\n", filename);
    return 1;
  }

  fs_mv(".beargit/.newindex", ".beargit/.index");
  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the project spec.
 *
 */

const char* go_bears = "THIS IS BEAR TERRITORY!";

int is_commit_msg_ok(const char* msg) {
  int i = 0;
  while (*(msg + i) != '\0') {
    int k = 0;
    if (*(msg + i) == *go_bears) {
      int j = i;
      while (*(msg+j)!='\0' && *(go_bears+k)!='\0' && *(msg+j)==*(go_bears+k)) {
        if (k == 22) {
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
 * You will need a destination string buffer to hold your next_commit_id,
   before you copy it back to commit_id
 * You will need to use a function we have provided for you.
 */

char dst_buf[COMMIT_ID_SIZE];

void next_commit_id(char* commit_id) {
  char branch_commit[BRANCHNAME_SIZE + COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.current_branch", branch_commit, BRANCHNAME_SIZE);
  strcat(branch_commit, commit_id);
  cryptohash(branch_commit, dst_buf);

}


int beargit_commit(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR:  Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char branch_name[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", branch_name, BRANCHNAME_SIZE);
  if (branch_name[0] == '\0') {
    fprintf(stderr, "ERROR:  Need to be on HEAD of a branch to commit.\n");
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  char new_dir[10 + COMMIT_ID_SIZE] = ".beargit/"; 
  strcat(new_dir, dst_buf);  
  fs_mkdir(new_dir);
  strcat(new_dir, "/");

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

const char null_id[COMMIT_ID_SIZE] = "0000000000000000000000000000000000000000";

int beargit_log(int limit) {
  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);

  if (!strcmp(commit_id, null_id)) {
    fprintf(stderr, "ERROR:  There are no commits.\n");
    return 1;
  }

  char commit_msg[MSG_SIZE];
  char dst_base[10] = ".beargit/";
  char dst_msg[14 + COMMIT_ID_SIZE];
  char dst_prev[15 + COMMIT_ID_SIZE];
  
  while (limit > 0 && strcmp(commit_id, null_id)) {
    
    strcpy(dst_msg, dst_base);
    strcat(dst_msg, commit_id);
    strcat(dst_msg, "/.msg");
    read_string_from_file(dst_msg, commit_msg, MSG_SIZE);

    fprintf(stdout, "commit %s\n", commit_id);
    fprintf(stdout, "   %s\n\n", commit_msg);

    strcpy(dst_prev, dst_base);
    strcat(dst_prev, commit_id);
    strcat(dst_prev, "/.prev");
    strcpy(commit_id, "");
    strcpy(commit_msg, "");
    strcpy(dst_msg, "");
    read_string_from_file(dst_prev, commit_id, COMMIT_ID_SIZE);
    strcpy(dst_prev, "");
    limit--;
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
  char curr_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", curr_branch, BRANCHNAME_SIZE);

  FILE* fbranches = fopen(".beargit/.branches", "r");
  
  char line[BRANCHNAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, curr_branch)) {
      fprintf(stdout, "   %s\n", line);
    } else {
      fprintf(stdout, "*  %s\n", line);
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
  FILE* findex = fopen(".beargit/.index", "r");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    fs_rm(line);
  }

  fclose(findex);

  if (strcmp(commit_id, null_id)) {
    
    char checkout_dir[10 + COMMIT_ID_SIZE] = ".beargit/"; 
    strcat(checkout_dir, commit_id);
    strcat(checkout_dir, "/");

    char checkout_file[9 + COMMIT_ID_SIZE + FILENAME_SIZE];
    strcpy(checkout_file, checkout_dir);
    strcat(checkout_file, ".index");
    fs_cp(checkout_file, ".beargit/.index");
    strcpy(checkout_file, checkout_dir);

    FILE* findex = fopen(".beargit/.index", "r");

    char line[FILENAME_SIZE];
    while(fgets(line, sizeof(line), findex)) {
      strtok(line, "\n");
      strcat(checkout_file, line);
      fs_cp(checkout_file, line);
      strcpy(checkout_file, checkout_dir);
    }

    fclose(findex);
  } else {

    FILE* findex = fopen(".beargit/.index", "w");
    fclose(findex);
  }

  write_string_to_file(".beargit/.prev", commit_id);

  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  char subdir[9 + COMMIT_ID_SIZE] = ".beargit/";
  strcat(subdir, commit_id);
  return fs_check_dir_exists(subdir);
  //return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // If not detached, leave the current branch by storing the current HEAD into that branch's file...
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }

   // Check whether the argument is a commit ID. If yes, we just change to detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    // ...and setting the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "");

    return checkout_commit(arg);
  }



  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(arg) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR:  A branch named %s already exists.\n", arg);
    return 1;
  } else if (!branch_exists && !new_branch) {
    fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
    return 1;
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // File for the branch we are changing into.
  char branch_file[17 + BRANCHNAME_SIZE] = ".beargit/.branch_";
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

int file_in_index(const char* filename, const char* index) {
  FILE* findex = fopen(index, "r");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (!strcmp(filename, line)) {
      return 1;
    }
  }

  fclose(findex);

  return 0;
}

int beargit_reset(const char* commit_id, const char* filename) {
  if (!is_it_a_commit_id(commit_id)) {
      fprintf(stderr, "ERROR:  Commit %s does not exist.\n", commit_id);
      return 1;
  }

  // Check if the file is in the commit directory
  char commit_dir[10 + COMMIT_ID_SIZE] = ".beargit/";
  strcat(commit_dir, commit_id);
  strcat(commit_dir, "/");

  char commit_index[16 + COMMIT_ID_SIZE];
  strcpy(commit_index, commit_dir);
  strcat(commit_index, ".index");

  if (!file_in_index(filename, commit_index)) {
    fprintf(stderr, "ERROR:  %s is not in the index of commit %s.\n", filename, commit_id);
    return 1;
  }

  // Copy the file to the current working directory
  char commit_file[9 + COMMIT_ID_SIZE + FILENAME_SIZE];
  strcpy(commit_file, commit_dir);
  strcat(commit_file, filename);
  fs_cp(commit_file, filename);

  // Add the file if it wasn't already there
  if (!file_in_index(filename, ".beargit/.index")) {
      FILE* findex = fopen(".beargit/.index", "a");
      fprintf(findex, "%s\n", filename);
      fclose(findex);
  }

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
  char commit_dir[10 + COMMIT_ID_SIZE] = ".beargit/";
  strcat(commit_dir, commit_id);
  strcat(commit_dir, "/");

  char commit_index[16 + COMMIT_ID_SIZE];
  strcpy(commit_index, commit_dir);
  strcat(commit_index, ".index");

  char commit_file[9 + COMMIT_ID_SIZE + FILENAME_SIZE];

  char extension_file[FILENAME_SIZE + COMMIT_ID_SIZE];

  FILE* fcommitindex = fopen(commit_index, "r");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fcommitindex)) {
    strtok(line, "\n");
    if (file_in_index(line, ".beargit/.index")) {
      strcpy(commit_file, commit_dir);
      strcat(commit_file, line);

      strcpy(extension_file, line);
      strcat(extension_file, ".");
      strcat(extension_file, commit_id);

      fs_cp(commit_file, extension_file);

      fprintf(stdout, "%s conflicted copy created\n", line);

    } else {
      strcpy(commit_file, commit_dir);
      strcat(commit_file, line);
      fs_cp(commit_file, line);

      FILE* findex = fopen(".beargit/.index", "a");
      fprintf(findex, "%s\n", line);
      fclose(findex);

      fprintf(stdout, "%s added\n", line);

    }
  }

  fclose(fcommitindex);

  return 0;
}

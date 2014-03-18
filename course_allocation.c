

/* HEADER FILES */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

void *student_thread(void *arg);

/* MCOM : 0 , PHD : 1 , BCOM : 2, ARTS : 3*/

struct student{
	int sid;
	int sbranch;
	int pref_array[8];
	int assign_array[4];
	int cassigned;
};

/* Commerce : 0 , Humanities :1, Management : 2, Arts : 3 */
/* Student branch enrolled : 0index-MCOM, 1index-PHD, 2index-BCOM, 3Index-ARTS*/
struct course{
	int cid;
	int kSpectrum;
	int stuEnrolled;	
	int stu_branch[4];
};

struct course *c;
struct student *s;

sem_t mutex;

int main(int argc, char*argv[]){
	
	//Improper Number of Arguments
        if(argc != 3){
                printf("Usage: <Executable> <# of Students> <# of Courses>\n");
                exit(1);
        }
	FILE *fd = NULL;	
	int i, j, k, p, q, res;
	int tstudents = atoi(argv[1]);	
	int tcourses = atoi(argv[2]);
	int carray[tcourses];
	for(i = 0 ; i < tcourses ; i++){
		carray[i] = 0;
	}
	
	//srand (time(NULL));
	
	c = (struct course *) malloc(sizeof(struct course) * tcourses);
	s = (struct student *) malloc(sizeof(struct student) * tstudents);
	fd = fopen("allocation.txt", "w");
	if(fd == NULL){
		perror("Cannot create file");
		exit(1);
	}
	
	pthread_t s_thread[tstudents];		
	
	sem_init(&mutex, 0, 1);
	
	for (i = 0 ; i < tcourses ; i++){
		/* INITIALIZE COURSE ID*/	
		c[i].cid = i;
		/* ASSIGN KNOWLEDGE SPECTRUM RANDOMLY -(ASSUME)*/
		int ks = i % 4;
		c[i].kSpectrum = ks;
		/* INITIAL STUDENT ENROLLED : MAX : 60*/
		c[i].stuEnrolled = 0;
		/* INITIALIZE THE STUDENT_BRANCH */
		c[i].stu_branch[0] = 12;
		c[i].stu_branch[1] = 12;	
		c[i].stu_branch[2] = 24;	
		c[i].stu_branch[3] = 12;
	}
	/*	
	for(i = 0; i < tcourses ;i++){
		printf("Course Id : %d\n",c[i].kSpectrum);
	}
	*/
	for(j = 0 ; j < tstudents ; j++){
		/* INITIALIZE STUDENT ID*/
		s[j].sid = i;
		/* ASSIGN BRANCH TO STUDENTS RANDOMLY - (ASSUME)*/
		int sb = rand() % 4;
		s[j].sbranch = sb;	
		/*ASSIGN COURSES TO THE PREFERENCE LIST */	
		int k = 0;
		while(1){
			int pi = rand() % tcourses;
			if(carray[pi] == 0){	
				if(k < 8 ){
					s[j].pref_array[k++] = pi; 
				}else{
					break;
				}	
			carray[pi] = 1;
			}
		}
		for(i = 0 ; i < tcourses ; i++){
                	carray[i] = 0;
   		}
		/*	
		printf("Student course preferences  : ");	
		for(k = 0 ; k < 8 ; k++){
			printf("%d ",s[j].pref_array[k]);
		}
		printf("\n");
		*/
		/* INITIAL ASSIGNED COURSES */	
		for(k = 0 ; k < 4 ; k++){
			s[j].assign_array[k] = 0;
		}
	
		s[j].cassigned = 4;
	}	
		
	for(p = 0 ; p < tstudents ; p++ ){
		res = pthread_create(&(s_thread[p]), NULL, student_thread , (void *) p);
	}	
	//printf("Waiting for the thread to finish");
	for(p = 0 ; p < tstudents ; p++ ){
                pthread_join(s_thread[p], NULL);
        }
	
	/* STUDENTS NOT ALLOCATED*/
	for(i = 0 ; i < tstudents ;i++){
		if(s[i].cassigned > 0){
		printf("STUDENT : %d NOT ASSIGNED.\n", i);
		}
	}	
	//	
	fprintf(fd, " COURSE ALLOCATION\n");
	fprintf(fd, " ------------------\n\n");	
	for(i = 0 ; i < tcourses ; i++){
		fprintf(fd, " %d. COURSE : %d HAS  %d STUDENTS.\n",i,c[i].cid,c[i].stuEnrolled);
	}
		
	fclose(fd);
return 0;
}
	
void *student_thread(void *arg1){
	int studentid = (int *)arg1;
	sem_wait(&mutex);
	//printf("Student Active :%d\n",studentid);
	int i;
	int m = 4;
	for(i = 0 ;i < 8 ; i++){
		int s_course_id = s[studentid].pref_array[i];	
		int s_branch = s[studentid].sbranch;
		int stu_branch_course = c[s_course_id].stu_branch[s_branch];

		if(stu_branch_course != 0){
			if(s[studentid].cassigned > 0){
				int x = m - s[studentid].cassigned ;
				s[studentid].assign_array[x] = s_course_id;
				s[studentid].cassigned--;
				c[s_course_id].stu_branch[s_branch]--;
				c[s_course_id].stuEnrolled++;
			}	
		}
	}	
	sem_post(&mutex);
}



/*
 *
 * allocation.c
 *
 * A program where data is allocated to various
 * "processes", and we keep track of the allocated
 * and unallocated memory.
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structures
typedef struct proc {
	char *id;
	int start;
	int end;
	int size;
} Process;

typedef struct hole {
	int start;
	int end;
	int size;
} Hole;

// Functions
void take_input(char *arr[]);
void alloc_proc(char *name, int size, char op);
Hole* first_fit(int size);
Hole* best_fit(int size);
Hole* worst_fit(int size);
void release(char *name);
void status();
void compaction();
int hole_count();
int occupied(int slot);
int find_hole_slot(int slot);

// Constants and global variables
#define LIMIT 32
int MAX;
Process *proc_arr[LIMIT];
Hole *hole_arr[LIMIT];

int main(int argc, char *argv[]) {

	// Taking command line input
	if (argc < 2) {
		printf("You did not enter an allocation limit.");
		return (1);
	}

	else {
		MAX = atoi(argv[1]);
	}

	size_t characters, buf_size = 64;
	char *input_arr[4], *input_buf, *token, final_str[buf_size];
	int i, index = 0;

	for (i = 0; i < 4; i++) {
		input_arr[i] = NULL;
	}

	for (i = 0; i < LIMIT; i++) {
		proc_arr[i] = NULL;
		hole_arr[i] = NULL;
	}

	// Create the first hole (spanning the entire memory space)
	hole_arr[0] = (Hole*) malloc(sizeof(Hole));

	hole_arr[0]->size = MAX;
	hole_arr[0]->start = 0;
	hole_arr[0]->end = MAX - 1;

	/*
	 * BEGINNING ENTRY SECTION
	 */

	printf("allocation> ");
	characters = getline(&input_buf, &buf_size, stdin);
	strcpy(final_str, input_buf);

	token = strtok(final_str, " \n");

	while (token != NULL) {
		input_arr[index] = token;

		index++;
		token = strtok(NULL, " \n");
	}

	/*
	 * ENDING ENTRY SECTION
	 */

	while (strcmp(input_arr[0], "Exit") != 0) {

		// Allocation request handler
		if (strcmp(input_arr[0], "RQ") == 0) {
			alloc_proc(input_arr[1], atoi(input_arr[2]), *input_arr[3]);
		}

		// Release request handler
		else if (strcmp(input_arr[0], "RL") == 0) {
			release(input_arr[1]);
		}

		// Compaction request handler
		else if (strcmp(input_arr[0], "C") == 0) {
			compaction();
		}

		// Status display request handler
		else if (strcmp(input_arr[0], "Status") == 0) {
			status();
		}

		/*
		 * Take next command
		 */
		index = 0;

		printf("allocation> ");
		characters = getline(&input_buf, &buf_size, stdin);
		strcpy(final_str, input_buf);

		token = strtok(final_str, " \n");

		while (token != NULL) {
			input_arr[index] = token;

			index++;
			token = strtok(NULL, " \n");
		}
	}

	return 0;
}

void alloc_proc(char *name, int size, char op) {
	int i, exist_flag = 0;

	// Check if any hole exists that is large enough for to fit process
	for (i = 0; i < LIMIT; i++) {
		if (hole_arr[i] != NULL) {
			if (hole_arr[i]->size >= size) {
				exist_flag = 1;
				break;
			}

		}
	}

	if (exist_flag == 1) {
		Hole *fill = NULL;

		// First fit
		if (op == 'F') {
			fill = first_fit(size);
		}

		else if (op == 'B') {
			fill = best_fit(size);
		}

		else if (op == 'W') {
			fill = worst_fit(size);
		}

		else {
			printf("Incompatible search algorithm entered.\n");
			return;
		}

		// Filling the hole / Creating new process
		Process *to_create = (Process*) malloc(sizeof(Process));

		to_create->id = (char*) malloc(5 * sizeof(char));
		strcpy(to_create->id, name);
		to_create->start = fill->start;
		to_create->end = to_create->start + size - 1;
		to_create->size = size;

		for (i = 0; i < LIMIT; i++) {
			if (proc_arr[i] == NULL) {
				proc_arr[i] = to_create;
				break;
			}
		}

		// Updating the hole
		fill->start += size;
		fill->size -= size;

		// Delete the hole if it has been fully filled
		if (fill->size <= 0) {
			for (i = 0; i < LIMIT; i++) {
				if (hole_arr[i] != NULL) {
					if (hole_arr[i]->start == fill->start) {
						free(hole_arr[i]);
						hole_arr[i] = NULL;
						break;
					}
				}
			}
		}

		printf("Successfully allocated %d to process %s\n", size, name);

	}

	else {
		printf("Could not allocate memory - no hole large enough.\n");
	}
}

Hole* first_fit(int size) {
	Hole *to_fill = NULL;
	int i, min_start = MAX;

	for (i = 0; i < LIMIT; i++) {
		if (hole_arr[i] != NULL) {
			if (hole_arr[i]->size >= size && hole_arr[i]->start < min_start) {
				to_fill = hole_arr[i];
				min_start = hole_arr[i]->start;
			}
		}
	}

	return to_fill;
}

Hole* best_fit(int size) {
	Hole *to_fill = NULL;
	int i, hole_num, delta_best = MAX, delta, index = 0;

	hole_num = hole_count();

	// If there is only one hole, then place it in there
	if (hole_num == 1) {
		for (i = 0; i < LIMIT; i++) {
			if (hole_arr[i] != NULL) {
				to_fill = hole_arr[i];
				return to_fill;
			}
		}
	}

	else if (hole_num > 1) {
		for (i = 0; i < LIMIT; i++) {
			if (hole_arr[i] != NULL) {
				if (hole_arr[i]->size >= size) {
					delta = hole_arr[i]->size - size;

					printf("index = %d delta = %d best delta = %d\n", index,
							delta, delta_best);

					if (delta < delta_best) {
						delta_best = delta;
						to_fill = hole_arr[i];
					}
					index++;
				}

			}
		}
	}

	return to_fill;
}

Hole* worst_fit(int size) {
	Hole *to_fill = NULL;
	int i, hole_num, delta_best = 0, delta, index = 0;

	hole_num = hole_count();

	// If there is only one hole, then place it in there
	if (hole_num == 1) {
		for (i = 0; i < LIMIT; i++) {
			if (hole_arr[i] != NULL) {
				to_fill = hole_arr[i];
				return to_fill;
			}
		}
	}

	else if (hole_num > 1) {
		for (i = 0; i < LIMIT; i++) {
			if (hole_arr[i] != NULL) {
				if (hole_arr[i]->size >= size) {
					delta = hole_arr[i]->size - size;

					printf("index = %d delta = %d best delta = %d\n", index,
							delta, delta_best);

					if (delta > delta_best) {
						delta_best = delta;
						to_fill = hole_arr[i];
					}
					index++;
				}
			}
		}
	}

	return to_fill;
}

void release(char *name) {
	// Find the process to remove

	int proc_ind = -1, i;

	for (i = 0; i < LIMIT; i++) {
		if (proc_arr[i] != NULL) {
			if (strcmp(proc_arr[i]->id, name) == 0) {
				proc_ind = i;
				break;
			}
		}

	}

	if (proc_ind > -1) {
		printf("Releasing memory for process %s\n", name);

		int left_slot, right_slot, left_occ, right_occ;

		left_slot = proc_arr[proc_ind]->start - 1;
		right_slot = proc_arr[proc_ind]->end + 1;

		left_occ = occupied(left_slot);
		right_occ = occupied(right_slot);

		/*
		 * 4 cases of hole management
		 */

		// If the process to remove is bordered by 2 processes
		if (left_occ == 1 && right_occ == 1) {
			Hole *to_create = (Hole*) malloc(sizeof(Hole));

			to_create->start = proc_arr[proc_ind]->start;
			to_create->end = proc_arr[proc_ind]->end;
			to_create->size = proc_arr[proc_ind]->size;

			for (i = 0; i < LIMIT; i++) {
				if (hole_arr[i] == NULL) {
					hole_arr[i] = to_create;
					break;
				}
			}
		}

		else if (left_occ == 0 && right_occ == 0) {
			int l_hole_ind, r_hole_ind;

			l_hole_ind = find_hole_slot(left_slot);
			r_hole_ind = find_hole_slot(right_slot);

			hole_arr[l_hole_ind]->end = hole_arr[r_hole_ind]->end;
			hole_arr[l_hole_ind]->size += proc_arr[proc_ind]->size
					+ hole_arr[r_hole_ind]->size;

			free(hole_arr[r_hole_ind]);
			hole_arr[r_hole_ind] = NULL;
		}

		else if (left_occ == 0) {
			int hole_ind;

			hole_ind = find_hole_slot(left_slot);

			hole_arr[hole_ind]->end = proc_arr[proc_ind]->end;
			hole_arr[hole_ind]->size += proc_arr[proc_ind]->size;
		}

		else if (right_occ == 0) {
			int hole_ind;

			hole_ind = find_hole_slot(right_slot);

			hole_arr[hole_ind]->start = proc_arr[proc_ind]->start;
			hole_arr[hole_ind]->size += proc_arr[proc_ind]->size;
		}

		free(proc_arr[proc_ind]);
		proc_arr[proc_ind] = NULL;

		printf("Successfully released memory for process %s\n", name);
	}

	else {
		printf("That process does not exist, and cannot be released.\n");
		return;
	}

}

void status() {
	int i, allocated = 0, unallocated;

	for (i = 0; i < LIMIT; i++) {
		if (proc_arr[i] != NULL) {
			allocated += proc_arr[i]->size;
		}
	}

	unallocated = MAX - allocated;

	printf("Partitions [Allocated memory = %d]:\n", allocated);

	for (i = 0; i < LIMIT; i++) {
		if (proc_arr[i] != NULL) {
			printf("Address [%d:%d] Process %s\n", proc_arr[i]->start,
					proc_arr[i]->end, proc_arr[i]->id);
		}
	}

	printf("\nHole [Free memory = %d]\n", unallocated);

	for (i = 0; i < LIMIT; i++) {
		if (hole_arr[i] != NULL) {
			printf("Address [%d:%d] len = %d\n", hole_arr[i]->start,
					hole_arr[i]->end, hole_arr[i]->size);
		}
	}
}

void compaction() {
	int i = 0, reloc = 0;

	for (i = 0; i < LIMIT; i++) {
		if (proc_arr[i] != NULL) {
			proc_arr[i]->start = reloc;
			proc_arr[i]->end = reloc + proc_arr[i]->size - 1;

			reloc += proc_arr[i]->size;
		}

		if (hole_arr[i] != NULL) {
			free(hole_arr[i]);
			hole_arr[i] = NULL;
		}
	}

	hole_arr[0] = (Hole*) malloc(sizeof(Hole));

	hole_arr[0]->start = reloc;
	hole_arr[0]->end = MAX - 1;
	hole_arr[0]->size = MAX - reloc;

	printf("Compaction process successful\n");

}

/*
 *
 * HELPER METHODS BELOW
 *
 */

int hole_count() {
	int total = 0;

	for (int i = 0; i < LIMIT; i++) {
		if (hole_arr[i] != NULL) {
			total++;
		}
	}

	return total;
}

/*
 * An integer that tells you whether a given
 * memory slot contains the start or end of
 * a process.
 */

int occupied(int slot) {
	if (slot < 0 || slot >= MAX) {
		return 1;
	}

	for (int i = 0; i < LIMIT; i++) {
		if (proc_arr[i] != NULL) {
			if (proc_arr[i]->start == slot || proc_arr[i]->end == slot) {
				return 1;
			}
		}
	}

	return 0;
}

int find_hole_slot(int slot) {
	for (int i = 0; i < LIMIT; i++) {
		if (hole_arr[i] != NULL) {
			if (hole_arr[i]->start == slot || hole_arr[i]->end == slot) {
				return i;
			}
		}
	}
}

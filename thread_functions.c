// thread_functions.c
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "hash_table.h"
#include "rwlock.h"
#include "timestamp.h"
#include "output.h"
#include "common.h"

void *insert_thread(void *arg) {
    int thread_id = *(int*)arg;
    Command *cmd = &commands[thread_id];

    // Get timestamp
    uint64_t timestamp = get_timestamp();

    // Write command to output.txt
    write_command_to_output(timestamp, "INSERT", cmd->name, cmd->salary);

    // Acquire write lock
    rwlock_acquire_writelock(&rwlock);

    // Log lock acquisition
    timestamp = get_timestamp();
    write_lock_event_to_output(timestamp, "WRITE LOCK ACQUIRED");

    // Perform insert operation
    hash_table_insert(cmd->name, cmd->salary);

    // Release write lock
    rwlock_release_writelock(&rwlock);

    // Log lock release
    timestamp = get_timestamp();
    write_lock_event_to_output(timestamp, "WRITE LOCK RELEASED");

    return NULL;
}

void *delete_thread(void *arg) {
    int thread_id = *(int*)arg;
    Command *cmd = &commands[thread_id];

    // Get timestamp
    uint64_t timestamp = get_timestamp();

    // Write command to output.txt
    write_command_to_output(timestamp, "DELETE", cmd->name, 0);

    // Acquire write lock
    rwlock_acquire_writelock(&rwlock);

    // Log lock acquisition
    timestamp = get_timestamp();
    write_lock_event_to_output(timestamp, "WRITE LOCK ACQUIRED");

    // Perform delete operation
    hash_table_delete(cmd->name);

    // Release write lock
    rwlock_release_writelock(&rwlock);

    // Log lock release
    timestamp = get_timestamp();
    write_lock_event_to_output(timestamp, "WRITE LOCK RELEASED");

    return NULL;
}

void *search_thread(void *arg) {
    int thread_id = *(int*)arg;
    Command *cmd = &commands[thread_id];

    // Get timestamp
    uint64_t timestamp = get_timestamp();

    // Write command to output.txt
    write_command_to_output(timestamp, "SEARCH", cmd->name, 0);

    // Acquire read lock
    rwlock_acquire_readlock(&rwlock);

    // Log lock acquisition
    timestamp = get_timestamp();
    write_lock_event_to_output(timestamp, "READ LOCK ACQUIRED");

    // Perform search operation
    hashRecord *record = hash_table_search(cmd->name);

    // Release read lock
    rwlock_release_readlock(&rwlock);

    // Log lock release
    timestamp = get_timestamp();
    write_lock_event_to_output(timestamp, "READ LOCK RELEASED");

    // Write search result
    if (record != NULL) {
        write_search_result(timestamp, record->name, record->salary);
    } else {
        write_no_record_found(timestamp);
    }

    return NULL;
}

void *print_thread(void *arg) {
    // Get timestamp
    uint64_t timestamp = get_timestamp();

    // Write command to output.txt
    write_command_to_output(timestamp, "PRINT", "", 0);

    // Acquire read lock
    rwlock_acquire_readlock(&rwlock);

    // Log lock acquisition
    timestamp = get_timestamp();
    write_lock_event_to_output(timestamp, "READ LOCK ACQUIRED");

    // Perform print operation
    pthread_mutex_lock(&output_mutex);
    FILE *fp = fopen("output.txt", "a");
    if (fp) {
        hash_table_print(fp);
        fclose(fp);
    }
    pthread_mutex_unlock(&output_mutex);

    // Release read lock
    rwlock_release_readlock(&rwlock);

    // Log lock release
    timestamp = get_timestamp();
    write_lock_event_to_output(timestamp, "READ LOCK RELEASED");

    return NULL;
}

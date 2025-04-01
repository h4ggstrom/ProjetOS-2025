#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/partition.h"

Test(partition, create_virtual_partition) {
    const char *file_path = "test_partition.img";
    const char *mount_point = "test_mount_point";
    int size_mb = 10;

    // Step 1: Create the virtual partition
    int result = create_virtual_partition(file_path, mount_point, size_mb);
    cr_assert_eq(result, 0, "Failed to create virtual partition");

    // Step 2: Verify that the partition file exists
    struct stat st;
    cr_assert_eq(stat(file_path, &st), 0, "Partition file does not exist");
    cr_assert(S_ISREG(st.st_mode), "Partition file is not a regular file");

    // Step 3: Verify that the mount point exists
    cr_assert_eq(stat(mount_point, &st), 0, "Mount point does not exist");
    cr_assert(S_ISDIR(st.st_mode), "Mount point is not a directory");

    // Step 4: Verify that the partition is mounted
    char command[256];
    snprintf(command, sizeof(command), "mount | grep %s > /dev/null", mount_point);
    result = system(command);
    cr_assert_eq(result, 0, "Partition is not mounted");

    // Cleanup: Unmount and delete the created files
    snprintf(command, sizeof(command), "sudo umount %s", mount_point);
    system(command);
    rmdir(mount_point);
    unlink(file_path);
}

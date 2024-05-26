// IOKit-Interactions by rsprbp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <IOKit/IOKitLib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <zlib.h> 
#include <signal.h> 

#define SERVICE_NAME "com.example.MyKernelService"
#define MAX_RETRY_ATTEMPTS 3
#define ENCRYPTION_KEY 0xAA
#define HISTORY_FILE "payload_history.log"

struct kernel_data_object {
    uint64_t data1;
    uint64_t data2;
    char description[256];
};

void logMessage(const char *message) {
    FILE *logFile = fopen("iokit-interactions.log", "a");
    if (logFile) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, message);
        fclose(logFile);
    } else {
        printf("Failed to open log file\n");
    }
}

void logPayloadHistory(const char *payload) {
    FILE *historyFile = fopen(HISTORY_FILE, "a");
    if (historyFile) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        fprintf(historyFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, payload);
        fclose(historyFile);
    } else {
        printf("Failed to open payload history, invalid file\n");
    }
}

void encryptDecryptPayload(char *payload) {
    for (size_t i = 0; i < strlen(payload); ++i) {
        payload[i] ^= ENCRYPTION_KEY;
    }
}

void compressPayload(char *payload, char **compressedPayload, size_t *compressedSize) {
    uLong srcLen = strlen(payload) + 1;
    uLong destLen = compressBound(srcLen);
    *compressedPayload = (char *)malloc(destLen);

    if (compress((Bytef *)*compressedPayload, &destLen, (const Bytef *)payload, srcLen) == Z_OK) {
        *compressedSize = destLen;
    } else {
        free(*compressedPayload);
        *compressedPayload = NULL;
        *compressedSize = 0;
    }
}

void decompressPayload(char *compressedPayload, size_t compressedSize, char **decompressedPayload) {
    uLong destLen = 256;
    *decompressedPayload = (char *)malloc(destLen);

    if (uncompress((Bytef *)*decompressedPayload, &destLen, (const Bytef *)compressedPayload, compressedSize) != Z_OK) {
        free(*decompressedPayload);
        *decompressedPayload = NULL;
    }
}

void secureFree(void *ptr, size_t size) {
    if (ptr) {
        memset(ptr, 0, size);
        free(ptr);
    }
}

void signalHandler(int signum) {
    printf("Signal %d received. Exiting.\n", signum);
    exit(signum);
}

void manipulateKernelDataWithPayload(struct kernel_data_object *obj, const char *payload) {
    if (!obj) {
        printf("Invalid kernel object pointer\n");
        logMessage("Invalid kernel object pointer");
        return;
    }
    obj->data1 = 0xdeadbeef; //lol you love to see this and you know it
    strncpy(obj->description, payload, sizeof(obj->description) - 1);
    obj->description[sizeof(obj->description) - 1] = '\0';
    printf("Manipulated kernel data:\n");
    printf("data1: 0x%llx\n", obj->data1);
    printf("description: %s\n", obj->description);
    logPayloadHistory(payload);
}

io_service_t getService() {
    io_service_t service = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching(SERVICE_NAME));
    if (!service) {
        logMessage("Failed to find service");
    }
    return service;
}

io_connect_t openConnection(io_service_t service) {
    io_connect_t connect = MACH_PORT_NULL;
    kern_return_t result = IOServiceOpen(service, mach_task_self(), 0, &connect);
    if (result != KERN_SUCCESS) {
        logMessage("Failed to open connection to service");
    }
    return connect;
}

void sendPayloadToKernel(io_connect_t connect, struct kernel_data_object *obj) {
    kern_return_t result = IOConnectCallStructMethod(connect, 0, obj, sizeof(*obj), NULL, 0);
    if (result != KERN_SUCCESS) {
        logMessage("Failed to send payload to kernel");
    } else {
        logMessage("Payload sent to kernel successfully");
    }
}

int main() {
    signal(SIGSEGV, signalHandler); // memory handling signal (still wip)

    io_service_t service = getService();
    if (!service) {
        return 1;
    }

    io_connect_t connect = openConnection(service);
    if (!connect) {
        IOObjectRelease(service);
        return 1;
    }

    struct kernel_data_object kernel_obj = {0};
    char *custom_payload = NULL;
    char *compressed_payload = NULL;
    size_t compressed_size = 0;
    int choice = 0;

    while (1) {
        printf("1. Default Payload\n");
        printf("2. Custom Payload\n");
        printf("3. Pattern Payload\n");
        printf("4. System Command\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 

        switch (choice) {
            case 1:
                manipulateKernelDataWithPayload(&kernel_obj, "Default Payload");
                sendPayloadToKernel(connect, &kernel_obj);
                break;
            case 2:
                printf("Enter custom payload: ");
                char customPayload[256];
                if (fgets(customPayload, sizeof(customPayload), stdin)) {
                    customPayload[strcspn(customPayload, "\n")] = '\0';
                    encryptDecryptPayload(customPayload);
                    manipulateKernelDataWithPayload(&kernel_obj, customPayload);
                    sendPayloadToKernel(connect, &kernel_obj);
                    encryptDecryptPayload(customPayload); // restore
                }
                break;
            case 3:
                printf("Enter a pattern for payload: ");
                char pattern[256];
                if (fgets(pattern, sizeof(pattern), stdin)) {
                    pattern[strcspn(pattern, "\n")] = '\0';
                    int repeat;
                    printf("Enter the number of times to repeat pattern: ");
                    scanf("%d", &repeat);
                    getchar(); 
                    custom_payload = (char *)malloc(strlen(pattern) * repeat + 1);
                    for (int i = 0; i < repeat; ++i) {
                        strcat(custom_payload, pattern);
                    }
                    manipulateKernelDataWithPayload(&kernel_obj, custom_payload);
                    sendPayloadToKernel(connect, &kernel_obj);
                    free(custom_payload);
                }
                break;
            case 4:
                printf("Enter a system command to execute: ");
                char system_command[256];
                if (fgets(system_command, sizeof(system_command), stdin)) {
                    system_command[strcspn(system_command, "\n")] = '\0';
                    system(system_command);
                }
                break;
            case 5:
                printf("Exiting program\n");
                IOServiceClose(connect);
                IOObjectRelease(service);
                return 0;
            default:
                printf("Invalid choice\n");
                break;
        }
    }

    IOServiceClose(connect);
    IOObjectRelease(service);
    return 0;
}

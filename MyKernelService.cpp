#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>

class com_example_MyKernelService : public IOService
{
    OSDeclareDefaultStructors(com_example_MyKernelService)

public:
    virtual bool init(OSDictionary *dictionary = nullptr) override;
    virtual void free() override;
    virtual IOService* probe(IOService *provider, SInt32 *score) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    virtual IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments *arguments, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference) override;

private:
    struct kernel_data_object {
        uint64_t data1;
        uint64_t data2;
        char description[256];
    };
    kernel_data_object kernel_obj;
};

bool com_example_MyKernelService::init(OSDictionary *dictionary)
{
    if (!IOService::init(dictionary)) {
        return false;
    }
    IOLog("MyKernelService::init\n");
    return true;
}

void com_example_MyKernelService::free()
{
    IOLog("MyKernelService::free\n");
    IOService::free();
}

IOService* com_example_MyKernelService::probe(IOService *provider, SInt32 *score)
{
    IOService *res = IOService::probe(provider, score);
    IOLog("MyKernelService::probe\n");
    return res;
}

bool com_example_MyKernelService::start(IOService *provider)
{
    if (!IOService::start(provider)) {
        return false;
    }
    IOLog("MyKernelService::start\n");
    registerService();
    return true;
}

void com_example_MyKernelService::stop(IOService *provider)
{
    IOLog("MyKernelService::stop\n");
    IOService::stop(provider);
}

IOReturn com_example_MyKernelService::externalMethod(uint32_t selector, IOExternalMethodArguments *arguments, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference)
{
    IOLog("MyKernelService::externalMethod\n");
    if (selector == 0 && arguments->structureInput && arguments->structureInputSize > 0) {
        const char *payload = static_cast<const char *>(arguments->structureInput);
        size_t payloadSize = arguments->structureInputSize;

        
        if (payloadSize <= sizeof(kernel_obj.description)) {
            strncpy(kernel_obj.description, payload, sizeof(kernel_obj.description) - 1);
            kernel_obj.description[sizeof(kernel_obj.description) - 1] = '\0';
            kernel_obj.data1 = 0xdeadbeef; // reference point 

            IOLog("Manipulated kernel data:\n");
            IOLog("data1: 0x%llx\n", kernel_obj.data1);
            IOLog("description: %s\n", kernel_obj.description);

            return kIOReturnSuccess;
        } else {
            IOLog("Payload size exceeds description buffer size\n");
            return kIOReturnBadArgument;
        }
    }
    return kIOReturnUnsupported;
}

OSDefineMetaClassAndStructors(com_example_MyKernelService, IOService)

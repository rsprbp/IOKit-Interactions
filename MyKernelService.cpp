#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>

class com_example_MyKernelService : public IOService
{
    OSDeclareDefaultStructors(com_example_MyKernelService)

public:
    virtual bool init(OSDictionary *dictionary = 0) override;
    virtual void free() override;
    virtual IOService* probe(IOService *provider, SInt32 *score) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments *arguments, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference) override;
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
        IOLog("Received payload: %s\n", (const char *)arguments->structureInput);
    
        return kIOReturnSuccess;
    }
    return kIOReturnUnsupported;
}

OSDefineMetaClassAndStructors(com_example_MyKernelService, IOService)

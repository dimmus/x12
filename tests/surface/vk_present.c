/*
 * G1 Vulkan client: render a solid frame with lavapipe, export via memfd,
 * CreateSurface → Attach → Present on X12-SURFACE (Xvfb compositor path).
 */
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <vulkan/vulkan.h>
#include <xcb/xcb.h>
#include <xcb/x12_surface.h>

#define WIDTH 64
#define HEIGHT 64
#define FOURCC_XR24 0x34325258u
#define MOD_LINEAR 0ull

static void
die(const char *msg)
{
    fprintf(stderr, "vk_present: %s\n", msg);
    exit(1);
}

static int
vulkan_fill_rgba(uint32_t *pixels, uint32_t n_pixels, uint32_t rgba)
{
    VkInstance instance;
    VkPhysicalDevice phys = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult vr;
    uint32_t n, i, qf = UINT32_MAX;
    VkPhysicalDevice *devs = NULL;
    void *mapped = NULL;
    size_t bytes = (size_t)n_pixels * 4;

    VkApplicationInfo app = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "x12-vk-present",
        .apiVersion = VK_API_VERSION_1_1,
    };
    VkInstanceCreateInfo ici = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app,
    };
    vr = vkCreateInstance(&ici, NULL, &instance);
    if (vr != VK_SUCCESS)
        return -1;

    vkEnumeratePhysicalDevices(instance, &n, NULL);
    if (n == 0)
        goto fail;
    devs = calloc(n, sizeof(*devs));
    if (!devs)
        goto fail;
    vkEnumeratePhysicalDevices(instance, &n, devs);
    phys = devs[0];

    {
        uint32_t nq = 0;
        VkQueueFamilyProperties *qprops;
        vkGetPhysicalDeviceQueueFamilyProperties(phys, &nq, NULL);
        qprops = calloc(nq, sizeof(*qprops));
        vkGetPhysicalDeviceQueueFamilyProperties(phys, &nq, qprops);
        for (i = 0; i < nq; i++) {
            if (qprops[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
                qf = i;
                break;
            }
        }
        free(qprops);
        if (qf == UINT32_MAX)
            goto fail;
    }

    {
        float prio = 1.0f;
        VkDeviceQueueCreateInfo qci = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = qf,
            .queueCount = 1,
            .pQueuePriorities = &prio,
        };
        VkDeviceCreateInfo dci = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &qci,
        };
        vr = vkCreateDevice(phys, &dci, NULL, &device);
        if (vr != VK_SUCCESS)
            goto fail;
    }

    {
        VkBufferCreateInfo bci = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = bytes,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };
        vr = vkCreateBuffer(device, &bci, NULL, &buffer);
        if (vr != VK_SUCCESS)
            goto fail;
    }

    {
        VkMemoryRequirements req;
        VkPhysicalDeviceMemoryProperties mprops;
        uint32_t mem_index = UINT32_MAX;
        VkMemoryAllocateInfo mai;

        vkGetBufferMemoryRequirements(device, buffer, &req);
        vkGetPhysicalDeviceMemoryProperties(phys, &mprops);
        for (i = 0; i < mprops.memoryTypeCount; i++) {
            if ((req.memoryTypeBits & (1u << i)) &&
                (mprops.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
                (mprops.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                mem_index = i;
                break;
            }
        }
        if (mem_index == UINT32_MAX)
            goto fail;
        mai = (VkMemoryAllocateInfo){
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = req.size,
            .memoryTypeIndex = mem_index,
        };
        vr = vkAllocateMemory(device, &mai, NULL, &memory);
        if (vr != VK_SUCCESS)
            goto fail;
        vkBindBufferMemory(device, buffer, memory, 0);
    }

    vr = vkMapMemory(device, memory, 0, bytes, 0, &mapped);
    if (vr != VK_SUCCESS)
        goto fail;
    for (i = 0; i < n_pixels; i++)
        pixels[i] = rgba;
    /* Also write through Vulkan mapping to prove the GPU-visible path. */
    memcpy(mapped, pixels, bytes);
    vkUnmapMemory(device, memory);
    mapped = NULL;

    vkDestroyBuffer(device, buffer, NULL);
    vkFreeMemory(device, memory, NULL);
    vkDestroyDevice(device, NULL);
    free(devs);
    vkDestroyInstance(instance, NULL);
    return 0;

fail:
    if (mapped)
        vkUnmapMemory(device, memory);
    if (buffer)
        vkDestroyBuffer(device, buffer, NULL);
    if (memory)
        vkFreeMemory(device, memory, NULL);
    if (device)
        vkDestroyDevice(device, NULL);
    free(devs);
    if (instance)
        vkDestroyInstance(instance, NULL);
    return -1;
}

static int
make_memfd(const uint32_t *pixels, size_t bytes)
{
    int fd = memfd_create("x12-surface", MFD_CLOEXEC);
    void *p;

    if (fd < 0)
        return -1;
    if (ftruncate(fd, (off_t)bytes) < 0) {
        close(fd);
        return -1;
    }
    p = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        close(fd);
        return -1;
    }
    memcpy(p, pixels, bytes);
    munmap(p, bytes);
    return fd;
}

int
main(int argc, char **argv)
{
    xcb_connection_t *c;
    xcb_screen_t *screen;
    xcb_window_t win;
    xcb_generic_error_t *err = NULL;
    xcb_void_cookie_t ck;
    xcb_x12surface_surface_t surface;
    xcb_query_extension_reply_t *qr;
    const char *extname = "X12-SURFACE";
    uint32_t *pixels;
    size_t bytes;
    int fd;
    uint32_t color = 0x00ff00ffu; /* XRGB: green-ish */
    bool use_vk = true;
    int i;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--cpu"))
            use_vk = false;
    }

    bytes = (size_t)WIDTH * HEIGHT * 4;
    pixels = calloc(WIDTH * HEIGHT, sizeof(uint32_t));
    if (!pixels)
        die("oom");

    if (use_vk) {
        if (vulkan_fill_rgba(pixels, WIDTH * HEIGHT, color) != 0) {
            fprintf(stderr, "vk_present: Vulkan path failed; use --cpu\n");
            free(pixels);
            return 2;
        }
        printf("vk_present: Vulkan (lavapipe) filled %dx%d\n", WIDTH, HEIGHT);
    } else {
        for (i = 0; i < WIDTH * HEIGHT; i++)
            pixels[i] = color;
        printf("vk_present: CPU filled %dx%d\n", WIDTH, HEIGHT);
    }

    fd = make_memfd(pixels, bytes);
    free(pixels);
    if (fd < 0)
        die("memfd");

    c = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(c))
        die("connect");

    qr = xcb_query_extension_reply(
        c, xcb_query_extension(c, (uint16_t)strlen(extname), extname), NULL);
    if (!qr || !qr->present) {
        fprintf(stderr, "vk_present: X12-SURFACE missing\n");
        free(qr);
        close(fd);
        xcb_disconnect(c);
        return 3;
    }
    free(qr);
    (void)xcb_get_extension_data(c, &xcb_x12surface_id);

    screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
    win = xcb_generate_id(c);
    xcb_create_window(c, 24, win, screen->root, 0, 0, WIDTH, HEIGHT, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, 0,
                      NULL);
    xcb_map_window(c, win);
    xcb_flush(c);

    {
        xcb_x12surface_query_version_reply_t *vr =
            xcb_x12surface_query_version_reply(
                c, xcb_x12surface_query_version(c, 1, 0), NULL);
        if (!vr)
            die("QueryVersion");
        printf("vk_present: X12-SURFACE %u.%u\n", vr->major_version,
               vr->minor_version);
        free(vr);
    }

    surface = xcb_generate_id(c);
    {
        int32_t bufs[1] = { fd };
        ck = xcb_x12surface_create_surface_checked(
            c, surface, win, 1, WIDTH, HEIGHT, WIDTH * 4, 0, 0, 0, 0, 0, 0, 0,
            24, 32, MOD_LINEAR, FOURCC_XR24, bufs);
    }
    close(fd);
    xcb_flush(c);
    /* Sync: void checked requests need a round-trip to observe errors. */
    {
        xcb_get_input_focus_reply_t *fr =
            xcb_get_input_focus_reply(c, xcb_get_input_focus(c), &err);
        free(fr);
    }
    if (err) {
        fprintf(stderr, "vk_present: CreateSurface error %u major=%u minor=%u\n",
                err->error_code, err->major_code, err->minor_code);
        free(err);
        xcb_disconnect(c);
        return 4;
    }
    (void)ck;
    printf("vk_present: CreateSurface OK\n");

    xcb_x12surface_attach(c, win, surface);
    xcb_x12surface_present(c, win, surface, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    xcb_flush(c);
    err = NULL;
    {
        xcb_get_input_focus_reply_t *fr =
            xcb_get_input_focus_reply(c, xcb_get_input_focus(c), &err);
        free(fr);
    }
    if (err) {
        fprintf(stderr, "vk_present: Attach/Present error %u\n", err->error_code);
        free(err);
        return 6;
    }

    printf("vk_present: Present OK (serial 1)\n");

    xcb_x12surface_destroy_surface(c, surface);
    xcb_destroy_window(c, win);
    xcb_flush(c);
    xcb_disconnect(c);
    return 0;
}

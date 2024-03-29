#pragma once

#include <WiFiClient.h>

#define MAX_PROTOCOL_LENGTH 1024

#define SEGMENT_BITS 0x7F
#define CONTINUE_BIT 0x80

enum McClientState
{
    MC_BEGIN = 0,
    MC_STATUS = 1,
    MC_LOGIN = 2,
    MC_GAME = 3,
};

struct McPacket
{
    int32_t length;
    int32_t packed_id;
    uint8_t data[MAX_PROTOCOL_LENGTH];
};

class McClient
{
public:
    McClient();
    McClient(WiFiClient client);
    ~McClient();

    bool connected();
    void stop();
    void handle();

private:
    WiFiClient client;
    McClientState state = MC_BEGIN;
    int32_t read_var_int(uint8_t *data, size_t *index);
    McPacket parseMcPacket(uint8_t *data, size_t *index);
};
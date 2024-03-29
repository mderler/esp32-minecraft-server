#include <mc_client.h>

McClient::McClient()
{
    this->client = WiFiClient();
}

McClient::McClient(WiFiClient client)
{
    this->client = client;
}

McClient::~McClient()
{
    client.stop();
}

void McClient::stop()
{
    client.stop();
}

bool McClient::connected()
{
    return client.connected();
}

int32_t McClient::read_var_int(uint8_t *data, size_t *index)
{
    int32_t value = 0;
    size_t position = 0;

    while (true)
    {
        uint8_t current_byte = data[(*index)++];
        value |= (current_byte & SEGMENT_BITS) << position;

        if ((current_byte & CONTINUE_BIT) == 0)
            break;

        position += 7;
    }

    return value;
}

McPacket McClient::parseMcPacket(uint8_t *data, size_t *index)
{
    McPacket packet;

    packet.length = read_var_int(data, index);

    Serial.print("Packet length: ");
    Serial.println(packet.length);

    size_t index_before_id = (*index);
    packet.packed_id = read_var_int(data, index);

    Serial.print("Packet ID: ");
    Serial.println(packet.packed_id);

    memcpy(packet.data, data + (*index), packet.length - ((*index) - index_before_id));

    (*index) += packet.length - ((*index) - index_before_id);

    return packet;
}

void McClient::handle()
{
    const int message_length = client.available();
    if (!message_length)
    {
        return;
    }
    Serial.print("Message length: ");
    Serial.println(message_length);

    uint8_t data[message_length];
    client.readBytes(data, message_length);

    size_t index = 0;

    while (index < message_length)
    {
        McPacket packet = parseMcPacket(data, &index);

        const int32_t protostate = (packet.packed_id << 3) | state;

        Serial.print("Packet state: ");
        Serial.println(protostate);
        switch (protostate)
        {
        case (0x00 << 3) | MC_BEGIN:
            Serial.println("Handshake");
            state = MC_LOGIN;
            break;
        case (0x00 << 3) | MC_LOGIN:
            Serial.println("Login");
            size_t name_index = 0;
            int32_t name_length = read_var_int(packet.data, &name_index);

            char username[name_length + 1];

            memcpy(username, &packet.data[name_index], name_length);

            username[name_length] = '\0';

            Serial.print("Username: ");
            Serial.println(username);

            uint64_t uuid1 = 0;
            uint64_t uuid2 = 0;

            memcpy(&uuid1, &packet.data[name_index + name_length], sizeof(uuid1));
            memcpy(&uuid2, &packet.data[name_index + name_length + sizeof(uuid1)], sizeof(uuid2));

            Serial.print("UUID: ");
            Serial.print(uuid1);
            Serial.print(" ");
            Serial.println(uuid2);

            break;
        }
    }
}
#include "SerialROS2.hpp"

char** split(char sep, char text[]) {
    char* token = nullptr;
    int len = 0;

    char** tokens = nullptr;
    int n_tokens = 0;

    for (int i = 0; i <= strlen(text); i++) {
        if (text[i] != '\n' && text[i] != sep) {
            token = (char*)realloc(token, sizeof(char) * (len + 1));
            token[len] = text[i];
            len++;
        } else {
            if (len > 0) {
                token = (char*)realloc(token, sizeof(char) * (len + 1));
                token[len] = '\0';

                tokens = (char**)realloc(tokens, sizeof(char*) * (n_tokens + 1));
                tokens[n_tokens] = token;
                n_tokens++;

                token = nullptr; 
                len = 0; 
            }
        }
    }
    
    tokens = (char**)realloc(tokens, sizeof(char*) * (n_tokens + 1));
    tokens[n_tokens] = nullptr;
                
    return tokens;
}

void freeSplit(char** t){
    for (int i = 0; t[i] != nullptr; i++) {
        free(t[i]);
    }
    free(t);
}

bool SerialROS2::init(){
    this->pc.set_baud(baud_rate);
    this->pc.set_blocking(false);
    return true;
}

DataRecv SerialROS2::recv(){
    DataRecv data_recv;
    data_recv.num_recv = pc.read(&data_recv.data, MAXIMUM_BUFFER_SIZE);
    return data_recv;
}

void SerialROS2::recvVals(char sep){
    DataRecv res = this->recv();
    float* recvs = nullptr;

    if (res.num_recv > 0){
        char** out = split(sep, res.data);
        for (int i = 0; out[i] != nullptr; i++) {
            recvs = (float*)realloc(recvs, sizeof(float) * (i+1));
            recvs[i] = atof(out[i]);
        }
        freeSplit(out);
        this->recvCallback(recvs); 
    } 

    free(recvs);
}

void SerialROS2::send(void* data_send, uint32_t size){
    pc.write(data_send, size);
}

void SerialROS2::attach(void (*func)(), chrono::milliseconds t=2000ms){
    send_timer.attach( callback(func), t);
}

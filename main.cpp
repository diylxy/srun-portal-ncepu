#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <cipher/md5.h>
#include <cipher/hmac.h>
#include <cipher/sha1.h>
#include <base/base64.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <string>

#include "xenEncode.h"
#include "httplib.h"

#define IFNAME_DEFAULT "eth0"
char ifname[IFNAMSIZ] = IFNAME_DEFAULT;

char token[100] = "";
char username[100] = "";
char password[100] = "";
char local_ip[100] = "";
///////////////////////////
const char ac_id[] = "1";
const char type[] = "1";
const char n[] = "200";
///////////////////////////
char xenEncodedBuffer[256];
char chksum_hex[41];
char password_HMAC_hex[33];

void calculatePasswordHMAC()
{
    char json_buffer[1024];
    char final_buffer[2048];
    char chksum[22];
    char xenEncodedBase64[256];
    uint8_t password_HMAC[16];

    sprintf(json_buffer, "{\"username\":\"%s\",\"password\":\"%s\",\"ip\":\"%s\",\"acid\":\"%s\",\"enc_ver\":\"srun_bx1\"}", username, password, local_ip, ac_id);

    memBlock *mem = xenEncode(json_buffer, token);
    std::string str = std::string((char *)mem->data, mem->capacity);
    freeMemBlock(mem);

    std::string b64 = "";
    Base64Encode(str, &b64);
    sprintf(xenEncodedBuffer, "{SRBX1}%s", b64.c_str());

    hmac_md5((uint8_t *)token, strlen(token), (uint8_t *)password, strlen(password), (uint8_t *)password_HMAC);
    for (int i = 0; i < 16; i++)
    {
        sprintf(password_HMAC_hex + i * 2, "%02x", (uint8_t)password_HMAC[i]);
    }
    password_HMAC_hex[32] = '\0';
    sprintf(final_buffer, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s", token, username, token, password_HMAC_hex, token, ac_id, token, local_ip, token, n, token, type, token, xenEncodedBuffer);
    SHA1Calc((uint8_t *)final_buffer, strlen(final_buffer), (uint8_t *)chksum);
    for (int i = 0; i < 20; i++)
    {
        sprintf(chksum_hex + i * 2, "%02x", (uint8_t)chksum[i]);
    }
    chksum_hex[40] = '\0';
}

bool authenticate()
{
    char url[1024];
    httplib::Client cli("http://202.204.67.15");
    sprintf(url, "/cgi-bin/get_challenge?callback=jQuery1124013750916444766936&username=%s&ip=%s", username, local_ip);
    auto res = cli.Get(url);
    if (res)
    {
        if (res->status == 200)
        {
            const char *token_start = strstr(res->body.c_str(), "challenge\":\"") + 12;
            memcpy(token, token_start, 64);
            printf("Challenge: %s\n", token);
            calculatePasswordHMAC();
            sprintf(url, "/cgi-bin/srun_portal?callback=jQuery1124013750916444766936&action=login&username=%s&password={MD5}%s&os=Windows+10&name=Windows&double_stack=0&chksum=%s&info=%s&ac_id=%s&ip=%s&n=%s&type=%s",
                    username, password_HMAC_hex, chksum_hex, xenEncodedBuffer, ac_id, local_ip, n, type);
            res = cli.Get(url);
            if (res)
            {
                if (res->status == 200)
                {
                    puts(res->body.c_str());
                    if (strstr(res->body.c_str(), "login_ok"))
                    {
                        printf("Login success\n");
                        return true;
                    }
                    else if (strstr(res->body.c_str(), "ip_already_online_error"))
                    {
                        printf("Already logged in\n");
                        return true;
                    }
                    else
                    {
                        printf("Login failed\n");
                    }
                }
            }
        }
        else
        {
            printf("Failed to get challenge\n");
        }
    }
    return false;
}

void getIPByIFace()
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    sprintf(local_ip, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    printf("Local IP: %s\n", local_ip);
}

bool connectivityTest()
{
    httplib::Client cli("http://4.ipw.cn");
    auto res = cli.Get("/");
    if (res)
    {
        if (res->status == 200)
        {
            if (res->body.c_str()[0] == '<')
                return false;
            else
                return true;
        }
    }
    return false;
}

int loop()
{
    static int err_cnt = 0;
    while (1)
    {
        while (connectivityTest() == false)
        {
            printf("Connectivity test failed!\n");
            while (authenticate() == false)
            {
                printf("Retry...%d\n", err_cnt);
                sleep(10);
                err_cnt++;
                if (err_cnt >= 3)
                {
                    err_cnt = 0;
                    if (ifname[0] != '\0')
                    {
                        getIPByIFace();
                    }
                    sleep(600);
                }
            }
            printf("OK!\n");
            err_cnt = 0;
            sleep(10);
        }
        sleep(60 * 5);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 4)
    {
        strcpy(username, argv[1]);
        strcpy(password, argv[2]);
        if (argv[3][0] >= '0' && argv[3][0] <= '9')
        {
            strcpy(local_ip, argv[3]);
            ifname[0] = '\0';
        }
        else
        {
            strcpy(ifname, argv[3]);
            getIPByIFace();
        }
    }
    else if (argc == 3)
    {
        strcpy(username, argv[1]);
        strcpy(password, argv[2]);
        getIPByIFace();
    }
    else
    {
        printf("Usage: %s <username> <password> [ipAddress | ifName]\n", argv[0]);
        return 1;
    }
    loop();
    return 0;
}

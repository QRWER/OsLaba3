#include <iostream>
#include <cmath>
#include <cstring>
#include <unistd.h>

struct FunctionData{
    double x;
    double y;
};

int pipe_in[2];
int pipe_out[2];
pid_t pid;

void Help() {
    std::cout << "Run program without option to calculate function.\n";
}

double ReadDouble(const std::string& msg) {
    std::cout << msg;
    double result;
    std::cin >> result;
    while(std::cin.fail() || std::cin.peek() != '\n') {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "MESSAGE\n";
        std::cin >> result;
    }
    return result;
}

double div(double numerator, int n) {
    for(int i = 1; i<=n; i++)
        numerator/=i;
    return numerator;
}

double Calculate(double x, double y) {
    double result = 0;
    double x0 = 1;
    double tgy = tan(y);
    for (int n = 0; n<=100; n++, x0 *= x){
        result += div((x0 + tgy + 2 * x * y), n+1);
    }
    return result;
}

void Frontend() {
    FunctionData data{};
    data.x = ReadDouble("Enter pointed x: ");
    data.y = ReadDouble("Enter pointed y: ");
    write(pipe_in[1], &data, sizeof(FunctionData));
    close(pipe_in[1]);
    double result;
    read(pipe_out[0], &result, sizeof(double));
    close(pipe_out[0]);
    std::cout << "Result: " << result << '\n';
    exit(0);
}

void Backend() {
    FunctionData data{};
    read(pipe_in[0], &data, sizeof(FunctionData));
    close(pipe_in[0]);
    double result = Calculate(data.x, data.y);
    write(pipe_out[1], &result, sizeof(double));
    close(pipe_out[0]);
}

int main(int argc, char* argv[]) {
    if(argc == 2 and !strcmp(argv[1], "--help")) {
        Help();
        exit(0);
    }
    else if (argc != 1)
        std::cout << "Run program with --help to get instruction\n";
    else {
        std::cout << "Program to calculate function:\n"
                  << "              n\n"
                  << "        100  x + tg(y) + 2xy\n"
                  << "f(x,y) = ∑   ―――――――――――――――\n"
                  << "        n=0      (n+1)!\n";
        pipe(pipe_in);
        pipe(pipe_out);
        pid = fork();
        if(pid < 0) {
            std::cerr << "Fatal error! Fork doesn't create\n";
            exit(1);
        }
        else if(pid > 0)
            Frontend();
        else
            Backend();
    }
    return 0;
}

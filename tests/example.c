int main() {
    void (*fun_ptr)(int) = &fun;
    (*fun_ptr)(10);

    for (c=0; c<3; c++){
        c = c + 1;
    }
    return c;
}

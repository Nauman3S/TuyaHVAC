void setupRelays()
{
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D8, OUTPUT);
    pinMode(D9, OUTPUT);
}

void controlRelay(int n, int v)
{
    if (n == 1)
    {
        digitalWrite(D5, v);
    }
    if (n == 2)
    {
        digitalWrite(D6, v);
    }
    if (n == 3)
    {
        digitalWrite(D8, v);
    }
    if (n == 4)
    {
        digitalWrite(D9, v);
    }
}
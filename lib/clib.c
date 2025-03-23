char decToHex(int dec) {
    if (dec < 10) return decToChar(dec);
    if (dec == 10) return 'a';
    if (dec == 11) return 'b';
    if (dec == 12) return 'c';
    if (dec == 13) return 'd';
    if (dec == 14) return 'e';
    if (dec == 15) return 'f';
    return '\0';
}

char decToChar(int dec) {
    return dec + 48;
}

int hexToDec(char hex) {
    if (hex >= '0' && hex <= '9') return charToDec(hex);
    if (hex == 'a' || hex == 'A') return 10;
    if (hex == 'b' || hex == 'B') return 11;
    if (hex == 'c' || hex == 'C') return 12;
    if (hex == 'd' || hex == 'D') return 13;
    if (hex == 'e' || hex == 'E') return 14;
    if (hex == 'f' || hex == 'F') return 15;
    return -1;
}

int charToDec(char c) {
    if (c == '0') return 0;
    if (c == '1') return 1;
    if (c == '2') return 2;
    if (c == '3') return 3;
    if (c == '4') return 4;
    if (c == '5') return 5;
    if (c == '6') return 6;
    if (c == '7') return 7;
    if (c == '8') return 8;
    if (c == '9') return 9;
    return -1;
}

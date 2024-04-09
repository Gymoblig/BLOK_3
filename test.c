int main() {
  // Otvorenie súboru v binárnom režime
  FILE *fp = fopen("koniec_poviedky.html", "rb");
  if (fp == NULL) {
    printf("Chyba pri otváraní súboru!\n");
    return 1;
  }

  // Deklarácia premenných
  char ch;
  int dešifrovaný_znak;

  // Èítanie znakov zo súboru a dešifrovanie
  while ((ch = fgetc(fp)) != EOF) {
    // Dešifrovanie znaku pomocou XOR s 27
    dešifrovaný_znak = ch ^ 27;

    // Výpis dešifrovaného znaku
    printf("%c", dešifrovaný_znak);
  }

  // Zatvorenie súboru
  fclose(fp);

  return 0;
}
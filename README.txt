====================================================================================================
                                                                                Sandu Geanina-Maria
                                                                                              324CC
                                      --- Tema 1 - Router ---


====================================================================================================

                                       --- Scopul temei ---

> Familiarizarea cu protocoale de layer 2 (ARP, Ethernet) si layer 3 (IP, ICMP), prin implementarea
    unui router cu functionalitati simple de packet exchange.

                                      --- Timp de lucru ---

> Cel putin 20 de ore, din cauza unor bug-uri care s-au dovedit a fi minore, din cauza ca C-ul cam
    da batai de cap - eg. folosirea de variabile de tip long, comutarea de packete cand nu se
    cunoaste adresa MAC destinatie (problema pe care nu am reusit sa o rezolv din cauza ca sequence
    number-ul pachetului din queue se schimba in mod inexplicabil, ceea ce rezulta print-un pachet
    duplicat, motiv pentru care testele de forwarding sunt picate).

====================================================================================================

                                      --- Implementare ---


>>> Parsarea tabelei de routare

        > Am citit fisierul rtable.txt linie cu linie, apeland pentru fiecare linie functia
            parse_routing_table, care adauga fiecare field al tabelei in campul corespunzator din
            structura routing_table_entry, rezultand un vector de structuri de acest tip.
        > Am sortat structura, folosind functia qsort in felul urmator: crescator dupa prefix, iar
            intrarile cu prefixe egale sortate descrescator dupa subnet mask, astfel obtinand adresa
            cea mai specifica pe prima intrare care corespunde prefixului.
        > Am realizat cautarea celei mai bune rute folosind un algoritm de cautare binara care
            intoarce prima aparitie a IP-ului cautat. Returnam intrarea care indeplineste conditia
            destination_ip & subnet_mask == prefix.


>>> ARP

        > Daca router-ul primeste un frame ARP, verificam campul operation al structurii.
        > Daca frame-ul este un ARP request, router-ul genereaza un ARP reply, apeland functia
            build_arp_header, in care isi adauga adresa MAC de pe interfata ceruta, apoi il trimite
            catre host-ul sursa.
        > Adauga IP-ul si MAC-ul host-ului sursa in tabela ARP (structura arp_table).


>>> Forwarding

        > Daca router-ul primeste un pachet IP pe care trebuie sa il trimita mai departe, verifica
            membrii headerului:
            - Daca TTL <= 1, router-ul trimite catre sursa un pachet de tip time exceeded (ICMP type
                11), apeland functia build_icmp_header cu parametrii corespunzatori (IP sursa /
                destinatie, type).
            - Daca checksum-ul pachetului este gresit (diferit de 0), router-ul da drop pachetului
                (continue).
        > Cautam apoi cea mai buna ruta catre IP-ul destinatie, apeland functia get_best_route
            - Daca nu se gaseste nicio ruta in tabela de rutare, trimite un pachet de tip
                destination unreachable sursei (type 3).
        > Verificam mai departe daca IP-ul destinatie corespunde unei intrari din ARP table. Daca
            nu, trimite un ARP request catre IP-ul destinatie. (aici am incercat sa adaug in queue
            pachetul primit si sa il trimit dupa primirea ARP reply-ului, dar am intampinat bug-ul
            ala tampit de care am zis mai sus si am reununtat).
        > Forwardeaza pachetul catre IP-ul destinatie, dupa ce scade campul TTL si ii modifica
            checksum-ul. (*Mentiune: aici a avut loc o mica eroare de logica din partea mea, pentru
            ca am trimis pachetul direct la IP-ul destinatie in loc sa il trimit la adresa next
            hop-ului, pentru ca in cazul de fata erau una si aceeasi. Mi-am dat seama totusi de asta
            putin cam tarziu si am vrut sa evit sa stau iar sa fac debugging, asa ca daca se poate
            va rog prefaceti-va ca am facut asta imi pare rau)


>>> ICMP

        > Daca router-ul a primit un pachet de tip ICMP Echo Request, verifica daca IP-ul destinatie
            este una din interfetele routerului (apeland functia is_router_interface).
        > Daca pachetul e destinat routerului, cream un nou pachet ICMP Echo Reply, construindu-i
            header-ul ICMP (type 0), IP si Ethernet.
        > Trimitem reply-ul pe interfata de pe care a venit request-ul.


====================================================================================================
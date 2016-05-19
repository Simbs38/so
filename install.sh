#!/bin/bash

cp -p sobucli /usr/bin/sobucli
cp -p sobusrv /usr/bin/sobusrv
clear

if [ -d "/home/$USER/.Backup" ]
then
    clear
    echo ""
    echo "Ja existe uma base de dados, deseja continuar a usar esta[Y/n]:"
    read op
    if [ "$op" == "n" -o "$op" == "N" ]
    then
        clear
        echo "Criar uma base da dados nova, tem a certeza que quer continuar?[Y/n]:"
        read op
        if [ ! "$op" == "n" -o "$op" == "N" ]
        then
            rm -r /home/$USER/.Backup
            mkdir /home/$USER/.Backup
            mkdir /home/$USER/.Backup/metadata
            mkdir /home/$USER/.Backup/data
        fi
    fi

else

mkdir /home/$USER/.Backup
mkdir /home/$USER/.Backup/metadata
mkdir /home/$USER/.Backup/data
fi
clear
echo ""
echo "----------------------------------------------------------"
echo "            Programa instalado com sucesso!"
echo ""
echo "----------------------------------------------------------"
echo " Por favor escreva \"sobusrv\" para inicializar o servidor"
echo ""
echo "----------------------------------------------------------"
echo "      Apos isto escreva \"sobucli\" <nome do ficheiro>"
echo "                 para guardar ficheiros."
echo ""
echo ""

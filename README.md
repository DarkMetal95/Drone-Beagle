# Todo


## Urgent:

* Mode debug
* Réception bluetooth code erreur : acknowledge


## Améliorations possibles:

* Ajouter diode de dysfonctionnement sur sortie GPIO BBB

# Liste des commandes
##Toutes les commandes:

	'k': Déconnection bluetooth
	'l': Extinction de la BBB
	'a': Activation du mode débug

Le message de la manette est le suivant (message à utiliser de manière à émuler la manette):

*Premier   octet => 0x55 - 'U'
*Deuxième  octet => Puissance Joystick 1
*Troisième octet => Angle Joystick 1
*Quatrième octet => Puissance Joystick 2
*Cinquième octet => Angle Joystick 2

##Pour paramétrer les coefficients:

Envoyer d'abord la valeur 0x01.

Puis envoyer la valeur sous forme d'un tableau de 5 char, dont le 5ème correspond au coefficient à modifier:
	0x00 : Kp
	0x01 : Ki
	0x02 : Kd

	[0x01]	['1']['0']['0']['0'][0x00] : Kp = 1000
	[0x01]	['3']['0']['0'][ 0 ][0x01] : Ki = 300
	[0x01]	['5']['0'][ 0 ][ 0 ][0x02] : Kd = 50

La valeur max est donc 9999.
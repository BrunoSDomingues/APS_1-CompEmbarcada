# APS 1 - Musical

## Projeto da disciplina de Computação Embarcada do Insper

Alunos:

- Bruno Domingues
- Thomas Queiroz

Realizado com o auxílio de:

- Rafael Corsi (professor)
- Eduardo Marossi (professor)
- Marco Alves (técnico)


### Proposta

O projeto tem como proposta o desenvolvimento de um sistema capaz de reproduzir músicas com o uso de um Buzzer. Além da reprodução, o sistema permite o usuário pausar a música e, caso deseje, trocar a música (três músicas disponíveis).

### Materiais

O projeto faz uso de um kit que consiste em:

- dois jumpers macho-fêmea;
- uma protoboard;
- um buzzer;
- um microcontrolador SAME70-Xplained;
- uma placa OLED1-Xplained.

### Ligações elétricas

![Ligações elétricas do projeto](ligacoes.jpg)

- **Placa OLED:** conectada nos pinos EXT1
- **Negativo do Buzzer:** GND da direita dos pinos EXT2
- **Positivo do Buzzer:** pino PC13 nos pinos EXT2


### Funcionalidade

- **Botão 1:** função play/pause
- **Botão 2:** função previous (música anterior)
- **Botão 3:** função next (próxima música)

- **LED verde:** pisca no ritmo da música

- **Display:** mostra o nome da música atual

### Vídeo do projeto funcionando

<a href="http://www.youtube.com/watch?feature=player_embedded&v=6HGn8RQK-t4
" target="_blank"><img src="http://i3.ytimg.com/vi/6HGn8RQK-t4/maxresdefault.jpg" 
alt="Vídeo do projeto" width="500" height="300"/></a>
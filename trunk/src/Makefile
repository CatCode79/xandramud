CC      = gcc
PROF    = -Wall -g3 -pedantic -ansi


# Per evitare di criptare le password dei player file.
NOCRYPT = -DNOCRYPT

# Da dichiarare se si vuole la compilazione MXP
MXP = -DMXP

# Da dichiare se non si vuole il multiplaying nel proprio mud.
#NOPLAYINGTWICE = -DNOPLAYINGTWICE

#Da utilizzare se non si hanno le funzioni srandon e random (che non sono Ansi-C)
OLD_RAND = -DOLD_RAND

#Compilazione del sistema meteo.
WEATHER = -DWEATHER


C_FLAGS = $(PROF) $(NOCRYPT) $(MXP) $(OLD_RAND) $(NOPLAYINGTWICE) $(WEATHER)
L_FLAGS = $(PROF)

O_FILES = act_comm.o act_hera.o act_info.o act_move.o act_obj.o act_wiz.o      \
          ban.o cabal.o comm.o const.o db.o dream.o effects.o fight.o flags.o  \
          handler.o healer.o interp.o lookup.o macro.o magic.o magic2.o        \
          marry.o martial_art.o mob_prog.o note.o obj_prog.o quest.o recycle.o \
          save.o skills.o special.o tables.o update.o weather.o

xandra: $(O_FILES)
	rm -f xandra
	$(CC) $(L_FLAGS) -o xandra $(O_FILES)

.c.o: xandra.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f *.o xandra

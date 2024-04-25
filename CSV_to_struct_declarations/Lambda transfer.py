import os
from shutil import copy
f = open("Digital Card Deck - Sheet1.csv","r")
Audio_file_dir = "Audio"
i=1

for line in f:
    words = line.split(",")
    words[1] = words[1].strip()
    f_out = open(str(i)+".txt","w")

    audio_base = words[3].rstrip(".wav\n")
    old_file_path = "Old_name_Audio/"+words[3].rstrip('\n')
    copy(old_file_path,"Audio/")
    old_file = os.path.join("Audio",words[3].rstrip('\n'))
    new_file = os.path.join("Audio",audio_base[0:8]+".wav")
    try:
        os.rename(old_file,new_file)
    except FileExistsError:
        new_file = new_file.rstrip(".wav")+"1"+".wav"

    f_out.write(words[0]+'\n')
    f_out.write(words[1]+'\n')
    
    f_out.write("Audio/"+new_file[6:]+'\n')

    f_out.close()
    i+=1

f = open("Digital Card Deck - Sheet1.csv","r")
Audio_file_dir = "Audio"
i=1

for line in f:
    words = line.split(",")
    words[1] = words[1].strip()
    f_out = open(str(i)+".txt","w")

    f_out.write(words[0]+'\n')
    f_out.write(words[1]+'\n')
    f_out.write(Audio_file_dir+'/'+words[3]+'\n')

    f_out.close()
    i+=1

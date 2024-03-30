f = open("Digital Card Deck - Sheet1.csv","r")

i=1

for line in f:
    words = line.split(",")
    words[1] = words[1].strip()
    f_out = open(str(i)+".txt","w")

    f_out.write(words[0]+'\n')
    f_out.write(words[1]+'\n')
    f_out.write("placeholder.wav"+'\n')

    f_out.close()
    i+=1

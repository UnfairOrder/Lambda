f = open("Digital Card Deck - Sheet1.csv","r")

f_out = open("New Lambda.txt","a")

f.readline()

for line in f:
    words = line.split(",");
    words[1] = words[1].strip();
    f_out.write('{'+'"'+words[0]+'"'+','+'"'+words[1]+'"'+","+'"'+"placeholder.wav"+'"'+'},\n')

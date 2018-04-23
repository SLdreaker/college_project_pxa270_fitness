import requests
from bs4 import BeautifulSoup as bs

def GetListFromLine( str, i ):
    itemList = [i, str ]
    return itemList

def getWeb(id):
    res = requests.get('http://211.21.168.52/FOOD/%A5D%AD%B9%C3%FE.htm')
    soup = bs(res.text, "html.parser")
    b = []
    i = 0
    for a in soup.select('p[style="margin:0cm;margin-bottom:.0001pt;text-align:center"] > span[style="font-size:10.0pt;color:black"]'):
        if a.string[0].isdigit():
            i = i + 1
            b.append(GetListFromLine(a.string, str(i)))
    return dict(b)
##    gg = a.string[0]
##print(type(a.string[0]))
##print(type(gg))
##result = math.pow(2, 3)
##print(type(result))


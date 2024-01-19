global_variable=0

def main():
    for i in range(0,10):
        print(i)
    i=0
    while True:
        if(i==10):
           break
        print(i)
        i+=1
    if(i==10):
        print("i is 10")
    else:
        print("i is not 10")
    if(i!=10):
        print("i is not 10")
    else:
        print("i is 10")
    match i:
        case 10:
            print("i is 10")
        case _:
            print("i is not 10")
    try:
        print("try")
        raise 1
    except int:
        print("except")
    assert i==10
  
    global global_variable
    global_variable=1
    print(global_variable)
    return 




    
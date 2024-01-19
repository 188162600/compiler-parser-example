class Class1:
    property1: bool
    property2: int
    property3: float
    property4: str
    def __init__(self, property1: bool, property2: int, property3: float, property4: str):
        self.property1 = property1
        self.property2 = property2
        self.property3 = property3
        self.property4 = property4
    def __del__(self):
        pass
    def method1(self, property1: bool, property2: int, property3: float, property4: str):
        pass
    
class Class2:
    property1: Class1
    def __init__(self, property1: Class1):
        self.property1 = property1
    def __del__(self):
        pass
    def method1(self, property1: Class1):
        pass

class Class3<T>:
    property1: T
    def __init__(self, property1: T):
        self.property1 = property1
    def __del__(self):
        pass
    def method1(self, property1: T):
        pass


class Class3<T>:
    property1: T
    def __init__(self, property1: T):
        self.property1 = property1
    def __del__(self):
        pass
    def method1<U>(self, arg1: T,arg2: U):
        pass

def main():
    value1:int=0
    value2:float=0.0
    value3:bool=False
    value4:str=""




    
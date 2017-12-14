
class Entry:
    name = ""
    size = 0
    imports = []

    def description(self):
        desc_str = "%s is a %s %s worth $%.2f." % (self.name, self.color, self.kind, self.value)
        return desc_str
# your code goes here

car1 = Vehicle()
car2 = Vehicle()


# test code
# print(car1.description())
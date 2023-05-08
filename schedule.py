class Schedule:

    def __init__(self, schedule):
        self.schedule = schedule
    
    def __iter__(self):
        for s in self.schedule:
            yield s
    
    def __getitem__(self, item):
         return self.schedule[item]

    def sort(self):
        return Schedule(sorted(self.schedule))

    def __str__(self):
        return str(self.schedule)
APP_NAME=boruvka
OBJDIR=objs

REFDIR=ref

default: $(APP_NAME)

CXX = g++ -m64 -std=c++11
CXXFLAGS = -I. -O3 -Wall -fopenmp

.PHONY: dirs clean

dirs:
		/bin/mkdir -p $(OBJDIR)/

clean:
		/bin/rm -rf $(OBJDIR) *~ $(APP_NAME)

OBJS=$(OBJDIR)/main.o $(OBJDIR)/union_find.o $(OBJDIR)/boruvka_sequential.o ${OBJDIR}/boruvka_parallel.o ${OBJDIR}/boruvka_parallel_star.o ${OBJDIR}/boruvka_parallel_star2.o ${OBJDIR}/boruvka_parallel_edge.o ${OBJDIR}/graph.o $(OBJDIR)/graph_tests.o ${OBJDIR}/create_graph.o

$(APP_NAME): dirs $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@

$(OBJDIR)/main.o: CycleTimer.h

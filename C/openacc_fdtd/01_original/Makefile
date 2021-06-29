
CC   = mpicc
CXX  = mpic++
GCC  = gcc
RM  = rm -f
MAKEDEPEND = makedepend

CFLAGS    = -O3 
GFLAGS    = -Wall -O3 
CXXFLAGS  = $(CFLAGS)
LDFLAGS   = 

SRCS    = main.c setup.c config.c fdtd2d.c fdtd2d_sources.c output.cc bitmap.cc
TARGET = run
DISTTARGET = $(TARGET)_1.0.0

OBJS += $(filter %.o,$(SRCS:%.c=%.o))
OBJS += $(filter %.o,$(SRCS:%.cc=%.o))
OBJS += $(filter %.o,$(SRCS:%.cpp=%.o))


DEPENDENCIES = $(subst .o,.d,$(OBJS))


.PHONY: all
all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) $(OBJS) -o $@ $(LDFLAGS)

%.o : %.c
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(CC) $(CFLAGS) $(TARGET_ARCH)-c $<

%.o : %.cc
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -c $<

%.o : %.cpp
	$(call make-depend,$<,$@,$(subst .o,.d,$@))
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -c $<

.PHONY: dist
dist :
	mkdir -p $(DISTTARGET)
	@for h in `makedepend -Y -f- -- $(CXXFLAGS) -- $(SRCS) | grep -e ":" | sed -e "s/.*: //" | tr " " "\n" | sort | uniq` ; \
	do \
		cp -p $$h $(DISTTARGET); \
	done
	cp -p $(SRCS) $(DISTTARGET)
	cp -p Makefile $(DISTTARGET)
	tar -zcvf $(DISTTARGET).tar.gz $(DISTTARGET)
	rm -rf $(DISTTARGET)


.PHONY: clean
clean :
	$(RM) $(TARGET)
	$(RM) $(OBJS)
	$(RM) $(DEPENDENCIES)
	$(RM) *~



ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPENDENCIES)
endif

# $(call make-depend,source-file,object-file,depend-file)
define make-depend
  @$(GCC) -MM            \
          -MF $3         \
          -MP            \
          -MT $2         \
          $(GFLAGS)      \
          $(TARGET_ARCH) \
          $1
endef



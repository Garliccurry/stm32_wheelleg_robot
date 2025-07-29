# 定义需要递归编译的子目录列表
SUBDIRS := BareMetal FreeRTOS 

# 默认目标：编译所有子目录
all: $(SUBDIRS)

# 递归编译子目录（仅当存在Makefile时）
$(SUBDIRS):
	@if [ -f $@/Makefile ]; then \
		echo "Building $@..."; \
		$(MAKE) -C $@; \
	else \
		echo "Skipping $@ (no Makefile found)"; \
	fi

# 递归清理子目录（仅当存在Makefile时）
clean:
	@for dir in $(SUBDIRS); do \
		if [ -f $$dir/Makefile ]; then \
			echo "Cleaning $$dir..."; \
			$(MAKE) -C $$dir clean; \
		else \
			echo "Skipping $$dir (no Makefile found)"; \
		fi \
	done

# 声明伪目标
.PHONY: all clean $(SUBDIRS)
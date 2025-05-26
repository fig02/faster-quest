#if DEBUG_FEATURES
    include "$(BUILD_DIR)/src/code/fq/fq_debug.o"
#endif

    include "$(BUILD_DIR)/src/code/fq/fq.o"
    include "$(BUILD_DIR)/src/code/fq/fq_cs.o"
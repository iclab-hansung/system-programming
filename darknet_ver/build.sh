cd /app/darknet_ver
gcc -Wall -O2 -Ilib/darknet/include -Ilib/darknet/src -Isrc \
    main.c src/util.c src/queue.c \
    $(printf 'lib/darknet/src/%s.c ' gemm utils cuda deconvolutional_layer convolutional_layer \
      list image activations im2col col2im blas crop_layer dropout_layer maxpool_layer \
      softmax_layer data matrix network connected_layer cost_layer parser option_list \
      detection_layer route_layer upsample_layer box normalization_layer avgpool_layer \
      layer local_layer shortcut_layer logistic_layer activation_layer rnn_layer gru_layer \
      crnn_layer demo batchnorm_layer region_layer reorg_layer tree lstm_layer l2norm_layer \
      yolo_layer iseg_layer) \
    -lm -pthread -o infer.o
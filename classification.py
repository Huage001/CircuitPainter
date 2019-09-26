def classification(x,y):
    import numpy as np
    import matplotlib.pyplot as plt
    import tensorflow as tf
    from PIL import Image,ImageFilter
    with tf.Session() as sess:
        Resolution=32

        x_min=1920
        y_min=1080
        x_max=0
        y_max=0
        for xi in x:
            x_min=min(x_min,min(xi))
            x_max=max(x_max,max(xi))
        for yi in y:
            y_min=min(y_min,min(yi))
            y_max=max(y_max,max(yi))
        for i in range(0,len(x)):
            for j in range(0,len(x[i])):
                x[i][j]-=x_min
                y[i][j]-=y_min
        x_max=x_max-x_min
        y_max=y_max-y_min
        if x_max>y_max:
            bias=(x_max-y_max)/2
            for i in range(0,len(y)):
                for j in range(0,len(y[i])):
                    y[i][j]=y[i][j]+bias
            plt.ylim([0, x_max])
        else:
            bias=(y_max-x_max)/2
            for i in range(0,len(x)):
                for j in range(0,len(x[i])):
                    x[i][j]=x[i][j]+bias
            plt.xlim([0, y_max])

        for i in range(0,len(x)):
            plt.plot(x[i],y[i],linewidth=15,color='black')
        plt.axis('off')
        plt.savefig('./test.png')
        plt.close()

        im=Image.open('./test.png')
        im=im.transpose(Image.FLIP_TOP_BOTTOM)
        im=im.convert('1')
        im=im.resize((Resolution,Resolution),Image.ANTIALIAS)
        im=np.array(im).reshape([-1,Resolution*Resolution*1])
        #im=im/127.5-1

        saver = tf.train.import_meta_graph('./ClassificationModel_update.ckpt.meta')
        saver.restore(sess, './ClassificationModel_update.ckpt')
        pred = tf.get_collection('network-output')[0]
        graph = tf.get_default_graph()
        x = graph.get_operation_by_name('x').outputs[0]
        y_ = sess.run(pred, feed_dict={x: im})
        ans=np.argmax(y_, axis=1)[0]
        if (ans==3 or ans==1) and y_[0][ans]<0.85:
            ans=0
        return ans

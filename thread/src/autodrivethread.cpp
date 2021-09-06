#include "autodrivethread.h"
#include <iostream>
//#define DOUBLE_LINK_LIST
#define COMMON
namespace zjl
{
    Autodrivethread::Autodrivethread(cameraThread *camera,serialPortReadWriteThread *serialPortReadWriteThread):
            camera_(camera),serialPortReadWriteThread_(serialPortReadWriteThread)
    {
        park = new Park(serialPortReadWriteThread_);
        if(!park->initial()){
            std::cout<<"park initial fail"<<endl;
            exit(-1);
        }
        start(10000);
        std::cout<<"initial OK!!!"<<endl;
        sequence = 0;
        count = 0;
	head_node = DNode(red,NULL,NULL);
    }

    void Autodrivethread::process()
    {
        src = camera_->getFrame();
	srcImage = src.clone();

	if(srcImage.empty())
        {
       return; 
	}


//        std::cout<<(int)receiveData_.flag<<std::endl;

#ifdef DOUBLE_LINK_LIST
        receiveData_ = serialPortReadWriteThread_->getReceiveMsg();
        if(receiveData_.flag == 10)
        {
//            free(head_node);
//            free(blue_node);
//            free(green_node);
            head_ptr = NULL;
            head_node.next = NULL;

            status = 0;
//            std::cout<<"NULL"<<std::endl;
            return;
        }
        if(status == 0)
        {
            if(receiveData_.flag == 0||receiveData_.flag == 2)
            {
                setcolorpriority(blue);
                status = 1;
                std::cout<<"blue"<<std::endl;
            }
            else if(receiveData_.flag == 3)
            {
                setcolorpriority(green);
                status = 1;
                std::cout<<"green"<<std::endl;
            }
            if(receiveData_.flag == 4)
            {
                setcolorpriority(red);
                status = 1;
                std::cout<<"red"<<std::endl;
            }

        }
        if(sequence ==0)
        {
            if(head_ptr->next)
            {
                if(!park->alignment(srcImage,head_ptr->next->value))
                {
                    head_ptr = head_ptr->next;
                }
                else
                {
                    if(head_ptr->prev)
                    {
                        sequence = 1;
                    }
                }
            }
            else
            {
                head_ptr = &head_node;
            }

        }
        else if(sequence ==1)
        {
            if(!park->alignment(srcImage,head_ptr->value))
            {
                if(!park->alignment(srcImage,head_ptr->next->value))
                {
                    sequence = 0;
                    head_ptr = &head_node;
                }
            }
            else
            {
                if(head_ptr->prev)
                {
                    head_ptr = head_ptr->prev;
                }
                else
                {
                    sequence = 0;
                }

            }
        }

        if(head_ptr->next)
        {
//                std::cout<<head_ptr->next->value<<std::endl;
        }
#endif

#ifdef COMMON
        receiveData_ = serialPortReadWriteThread_->getReceiveMsg();
	//std::cout<<(int)receiveData_.flag<<std::endl;
	if(receiveData_.flag == 0||receiveData_.flag == 2)
            {
                park->alignment(srcImage,blue,0,(int)receiveData_.flag_sun);
  //              std::cout<<"blue"<<std::endl;
            }
            else if(receiveData_.flag == 3)
            {
                park->alignment(srcImage,green,(int)receiveData_.flag,(int)receiveData_.flag_sun);
               //std::cout<<"green"<<std::endl;
            }
            else if(receiveData_.flag == 4)
            {
                park->alignment(srcImage,red,(int)receiveData_.flag,(int)receiveData_.flag_sun);
                //std::cout<<"red"<<std::endl;
            }
            else if(receiveData_.flag == 1)
            {
                park->alignment(srcImage,gray,(int)receiveData_.flag,(int)receiveData_.flag_sun);
                //std::cout<<"gray"<<std::endl;
            }
	else
	{
//	std::cout<<"NULL"<<std::endl;

	}
#endif
    }

    void Autodrivethread::setcolorpriority(Color color_)
    {
        if(color_ == red)
        {
            head_ptr = &head_node;
            Nodeptr red_node =  new DNode(red,&head_node,NULL);
            head_node.next = red_node;
            Nodeptr green_node = new DNode(green,red_node,NULL);
            red_node->next = green_node;
            Nodeptr blue_node = new DNode(blue,green_node,NULL);
            green_node->next = blue_node;

        }
        else if (color_ == blue)
        {
            head_ptr = &head_node;
            Nodeptr blue_node = new DNode(blue,&head_node,NULL);
            head_node.next = blue_node;
//            Nodeptr green_node = new DNode(green,blue_node,NULL);
//            blue_node->next = green_node;
            Nodeptr red_node =  new DNode(red,blue_node,NULL);
            blue_node->next = red_node;

        }
        else if(color_ == green)
        {
            head_ptr = &head_node;
            Nodeptr green_node = new DNode(green,&head_node,NULL);
            head_node.next = green_node;
            Nodeptr red_node =  new DNode(red,green_node,NULL);
            green_node->next = red_node;
            Nodeptr blue_node = new DNode(blue,red_node,NULL);
            red_node->next = blue_node;
        }
    }


}

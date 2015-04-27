#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


typedef struct {

	int key_num;
	int *key_list;
	long *child;
}btree_node;

long queue[100000];
int que_front,que_back; //go in thru back, come out thru front


long parent_holder[100];
int level_count=-1;
int order=4;
long root_offset=-1;
int print_level_count=0;

int insert_node(int value, btree_node* root, FILE *index_file);
long find_insert_node(long cur_node_offset,int value,FILE* index_file);
int insert_promoted(long parent_offset,int value,long l_offset,long r_offset, FILE *index_file);
long find_node(long cur_node_offset,int value, FILE *index_file);
int print_tree(long cur_node_offset,FILE* index_file)
{	int i;
	if(cur_node_offset==root_offset)
	{
		print_level_count=0;
		//first element in queue
		//printf("\nfirst element\n");
		que_front=0;
		que_back=-1;
		queue[++que_back]=cur_node_offset;
		queue[++que_back]=-1;
		print_level_count++;
		printf("%2d: ",print_level_count);
		//print_level_count++;
	}
	btree_node cur_node;
	cur_node.key_list=(int *) calloc(order - 1, sizeof(int));
	cur_node.child=(long *) calloc(order, sizeof(long));
	
	while(que_front<=que_back) //queue not empty
	{
	cur_node_offset=queue[que_front];
	//que_front++;
	if(cur_node_offset==-1)
	{
		
		print_level_count++;
		que_front++;
		if(que_front<=que_back)
		{	printf("\n%2d: ",print_level_count);
			queue[++que_back]=-1;
			
		}
		
	}
	else
	{
	que_front++;
	fseek(index_file,cur_node_offset,SEEK_SET);
	fread(&(cur_node.key_num),sizeof(int),1,index_file);
	fread(cur_node.key_list,sizeof(int),(order-1),index_file);
	fread(cur_node.child,sizeof(long),order,index_file);
	//printf(" ");
	for(i=0;i<(cur_node.key_num-1);i++)
	{
		printf("%d,",cur_node.key_list[i]);
	}
	//print last element
	printf("%d ",cur_node.key_list[i]);
	if(cur_node.child[0]!=0)//child present
	{
	for(i=0;i<=(cur_node.key_num);i++)
	{
		queue[++que_back]=cur_node.child[i];
	}	
	}

	}
	

	}
	printf("\n");
	return 1;
}



int insert_node(int value, btree_node* root, FILE *index_file)
{
	int i;
	int length;
	int temp_list[order];
	int temp;
	//printf("\nin insert_node, root:%ld\n",root_offset);
//we have root
	if(root->child[0]==0) //implies no children in root
	{	
		//printf("\nroot no childern\n");
		//insert here
		
		for(i=0;i<(root->key_num);i++)
		{	
			temp_list[i]=root->key_list[i];
			//printf("\ntemp_list[%d]:%d\n", i,temp_list[i]);
			if(temp_list[i]==value)
				return (-1*value);
		}
		 length=i;
		i=0;
		while(i<length && value>temp_list[i] )
		{
			i++;
		}
		temp=i;
		for(i=length;i>temp;i--)
		{
			temp_list[i]=temp_list[i-1];
		}
		temp_list[temp]=value;


		if(length>=(order-1))
		{
			//overflow
					//printf("\nList is full\n");
					int m=ceil((float)(order-1)/2);
					//printf("\nm=%d, length:%d\n",m,length);
					//update left node and write to file in curloc of insert node
					// for(i=0;i<order;i++)
					// {
					// 	printf("\ntemp_list[%d]:%d\n", i,temp_list[i]);
					// }


					 for(i=0;i<m;i++)
							{
								root->key_list[i]=temp_list[i];
							}
					root->key_num=m;
					//printf("\ntemp_list[m]:%d\n",temp_list[m]);
					fseek(index_file,root_offset,SEEK_SET);
					fwrite(&(root->key_num),sizeof(int),1,index_file);
					fwrite(root->key_list,sizeof(int),order-1,index_file);
					fwrite(root->child,sizeof(long),order,index_file);
					int l_offset=root_offset;
					//make a new right node
					btree_node right_node;
					right_node.key_list=(int *) calloc(order - 1, sizeof(int));
					right_node.child=(long *) calloc(order, sizeof(long));
					right_node.key_num=length-m;
					int temp_count=0;
					for(i=m+1;i<=length;i++)
						{
							right_node.key_list[temp_count++]=temp_list[i];
						}
					fseek(index_file,0,SEEK_END); // seek to eof and write
					long r_offset=ftell(index_file); 
					fwrite(&(right_node.key_num),sizeof(int),1,index_file);
					fwrite(right_node.key_list,sizeof(int),order-1,index_file);
					fwrite(right_node.child,sizeof(long),order,index_file);
					fseek(index_file,0,SEEK_END);
					root_offset=ftell(index_file);
					btree_node new_root;
					new_root.key_list=(int *) calloc(order - 1, sizeof(int));
					new_root.child=(long *) calloc(order, sizeof(long));
					new_root.key_num=1;
					new_root.child[0]=l_offset;
					new_root.child[1]=r_offset;

					new_root.key_list[0]=temp_list[m];
					//printf("\nnew root val:%d, temp_list[m]:%d\n", new_root.key_list[0],temp_list[m]);
					fwrite(&(new_root.key_num),sizeof(int),1,index_file);
					fwrite(new_root.key_list,sizeof(int),order-1,index_file);
					fwrite(new_root.child,sizeof(long),order,index_file);
					return value;


		}
		else
		{
			//not full 
			for(i=0;i<=length;i++)
			{
				root->key_list[i]=temp_list[i];		
			}
				root->key_num=root->key_num+1;
				fseek(index_file,root_offset,SEEK_SET);
				fwrite(&(root->key_num),sizeof(int),1,index_file);
				fwrite(root->key_list,sizeof(int),order-1,index_file);
				fwrite(root->child,sizeof(long),order,index_file);
				return value;
		}

	}
	else
	{
		//root has children
		//printf("\nroot has children, need to find correct leaf to insert\n");
		long cur_node_offset=root_offset;
		//returns offset of the leaf where we need to insert
		long leaf_node_offset=find_insert_node(cur_node_offset,value,index_file);
		//printf("\nto be inserted at address %ld\n",leaf_node_offset );
		if(leaf_node_offset==-1)
		{
			//printf("\nduplicate\n");
			return (-1*value);
		}
		btree_node insertion_node;
		insertion_node.key_list=(int *) calloc(order - 1, sizeof(int));
		insertion_node.child=(long *) calloc(order, sizeof(long));
		fseek(index_file,leaf_node_offset,SEEK_SET);
		fread(&(insertion_node.key_num),sizeof(int),1,index_file);
		fread(insertion_node.key_list,sizeof(int),(order-1),index_file);
		fread(insertion_node.child,sizeof(long),order,index_file);
		//now node to be inserted has been got
		//make new temp list adding value to its current list
		for(i=0;i<(insertion_node.key_num);i++)
		{	
			temp_list[i]=insertion_node.key_list[i];
			//printf("\ntemp_list[%d]:%d\n", i,temp_list[i]);
			//an unnecessary condition now
			if(temp_list[i]==value)
				return (-1*value);
		}
		length=i;
		i=0;
		while(i<length && value>temp_list[i] )
		{
			i++;
		}
		temp=i;
		for(i=length;i>temp;i--)
		{
			temp_list[i]=temp_list[i-1];
		}
		temp_list[temp]=value;
		//sorted temp_list achieved
		if(length>=(order-1))
		{
			//over flow -> insert promoted node to parent
				//printf("\nList is full\n");
					int m=ceil((float)(order-1)/2);
					//printf("\nm=%d, length:%d\n",m,length);
					//update left node and write to file in curloc of insert node
					// for(i=0;i<order;i++)
					// {
					// 	printf("\ntemp_list[%d]:%d\n", i,temp_list[i]);
					// }


					 for(i=0;i<m;i++)
							{
								insertion_node.key_list[i]=temp_list[i];
							}
					insertion_node.key_num=m;
					//printf("\ntemp_list[m]:%d\n",temp_list[m]);
					fseek(index_file,leaf_node_offset,SEEK_SET);
					fwrite(&(insertion_node.key_num),sizeof(int),1,index_file);
					fwrite(insertion_node.key_list,sizeof(int),order-1,index_file);
					fwrite(insertion_node.child,sizeof(long),order,index_file);
					int l_offset=leaf_node_offset;

					btree_node right_node;
					right_node.key_list=(int *) calloc(order - 1, sizeof(int));
					right_node.child=(long *) calloc(order, sizeof(long));
					right_node.key_num=length-m;
					int temp_count=0;
					for(i=m+1;i<=length;i++)
						{
							right_node.key_list[temp_count++]=temp_list[i];
						}
					fseek(index_file,0,SEEK_END); // seek to eof and write
					long r_offset=ftell(index_file); 
					fwrite(&(right_node.key_num),sizeof(int),1,index_file);
					fwrite(right_node.key_list,sizeof(int),order-1,index_file);
					fwrite(right_node.child,sizeof(long),order,index_file);
					//now temp_list[m] has promted value, l_offset the left child addr and r_offset the right child addr
					long parent_offset=parent_holder[level_count];
					level_count--;
					return insert_promoted( parent_offset, temp_list[m], l_offset, r_offset, index_file);


		}
		else
		{
			//no overflow
		for(i=0;i<=length;i++)
			{
				insertion_node.key_list[i]=temp_list[i];		
			}
				insertion_node.key_num=insertion_node.key_num+1;
				fseek(index_file,leaf_node_offset,SEEK_SET);
				fwrite(&(insertion_node.key_num),sizeof(int),1,index_file);
				fwrite(insertion_node.key_list,sizeof(int),order-1,index_file);
				fwrite(insertion_node.child,sizeof(long),order,index_file);
				return value;	
		}
		return 1;
	}


}


long find_node(long cur_node_offset,int value, FILE *index_file)
{
	btree_node cur_node;
	cur_node.key_list=(int *) calloc(order - 1, sizeof(int));
	cur_node.child=(long *) calloc(order, sizeof(long));
	fseek(index_file,cur_node_offset,SEEK_SET);
	fread(&(cur_node.key_num),sizeof(int),1,index_file);
	fread(cur_node.key_list,sizeof(int),(order-1),index_file);
	fread(cur_node.child,sizeof(long),order,index_file);
	int s=0;
	if(cur_node.child[0]==0)
	{
		//is a leaf, here insertion takes place
		s=0;
		while(s<(cur_node.key_num))
		{
			if(value==(cur_node.key_list[s]))
				return value;
			s++;
		}
		return -1;
	}
	//not a leaf
	//need to traverese to one of the children
	//printf("\nChildren present; value:%d root->key_num:%d\n",value,cur_node.key_num);
	s=0;
	while(s<(cur_node.key_num))
	{
		if(value==(cur_node.key_list[s]))
			return value;
		else if(value<(cur_node.key_list[s]))
			break;
		else
			s++;
	}

	//printf("\n s is:%d\n",s );

	if(cur_node.child[s]!=0)
	{	
		//printf("\nchild exists\n");
		//level_count++;
		//printf("\nlevel count:%d\n",level_count);
		//parent_holder[level_count]=cur_node_offset;
		return find_node(cur_node.child[s],value,index_file);
	}
	else
	{	
		//make a new node , write it to file and return its address
		//is this case possible?? dont think so
		return cur_node_offset;
	}

}
long find_insert_node(long cur_node_offset,int value, FILE *index_file)
{
//cur_node_offset contains offset in file of current root
	//fetch the node from the file
	btree_node cur_node;
	cur_node.key_list=(int *) calloc(order - 1, sizeof(int));
	cur_node.child=(long *) calloc(order, sizeof(long));
	fseek(index_file,cur_node_offset,SEEK_SET);
	fread(&(cur_node.key_num),sizeof(int),1,index_file);
	fread(cur_node.key_list,sizeof(int),(order-1),index_file);
	fread(cur_node.child,sizeof(long),order,index_file);
	int s=0;
	if(cur_node.child[0]==0)
	{
		//is a leaf, here insertion takes place
		s=0;
		while(s<(cur_node.key_num))
		{
			if(value==(cur_node.key_list[s]))
				return -1;
			s++;
		}
		return cur_node_offset;
	}
	//not a leaf
	//need to traverese to one of the children
	//printf("\nChildren present; value:%d root->key_num:%d\n",value,cur_node.key_num);
	s=0;
	while(s<(cur_node.key_num))
	{
		if(value==(cur_node.key_list[s]))
			return -1;
		else if(value<(cur_node.key_list[s]))
			break;
		else
			s++;
	}

	//printf("\n s is:%d\n",s );

	if(cur_node.child[s]!=0)
	{	
		//printf("\nchild exists\n");
		level_count++;
		//printf("\nlevel count:%d\n",level_count);
		parent_holder[level_count]=cur_node_offset;
		return find_insert_node(cur_node.child[s],value,index_file);
	}
	else
	{	
		//make a new node , write it to file and return its address
		//is this case possible?? dont think so
		return cur_node_offset;
	}

}

int insert_promoted(long parent_offset,int value,long l_offset,long r_offset, FILE *index_file)
{	
	//printf("\nin insert_promoted, %d\n",value );
	//printf("\nlevlel count:%d",level_count);
	int temp_list[order],temp_child_list[order+1],temp_count,temp,i,s,length;
	btree_node cur_node;
	cur_node.key_list=(int *) calloc(order - 1, sizeof(int));
	cur_node.child=(long *) calloc(order, sizeof(long));
	fseek(index_file,parent_offset,SEEK_SET);
	fread(&(cur_node.key_num),sizeof(int),1,index_file);
	fread(cur_node.key_list,sizeof(int),(order-1),index_file);
	fread(cur_node.child,sizeof(long),order,index_file);
	for(i=0;i<(cur_node.key_num);i++)
		{	
			temp_list[i]=cur_node.key_list[i];
			temp_child_list[i]=cur_node.child[i];
			//printf("\ntemp_list[%d]:%d\n", i,temp_list[i]);
			//an unnecessary condition now
			if(temp_list[i]==value)
				return (-1*value);
		}
		length=i;
		temp_child_list[i]=cur_node.child[i];
		
		i=0;
		while(i<length && value>temp_list[i] )
		{
			i++;
		}
		temp=i;
		for(i=length;i>temp;i--)
		{
			temp_list[i]=temp_list[i-1];
			temp_child_list[i+1]=temp_child_list[i];
		}
		temp_list[temp]=value;
		temp_child_list[temp]=l_offset;
		temp_child_list[temp+1]=r_offset;
		//achieved sorted temp_list
		if(length>=(order-1))
		{
			//over flow -> insert promoted node to parent
					//printf("\nList is full\n");
					int m=ceil((float)(order-1)/2);
					//printf("\nm=%d, length:%d\n",m,length);
					//update left node and write to file in curloc of insert node
					// for(i=0;i<order;i++)
					// {
					// 	printf("\ntemp_list[%d]:%d\n", i,temp_list[i]);
					// }


				for(i=0;i<m;i++)
					{
					cur_node.key_list[i]=temp_list[i];
					cur_node.child[i]=temp_child_list[i];
					}
					cur_node.child[i]=temp_child_list[i];
					int child_marker=i;

					cur_node.key_num=m;
					//printf("\ntemp_list[m]:%d\n",temp_list[m]);
					fseek(index_file,parent_offset,SEEK_SET);
					fwrite(&(cur_node.key_num),sizeof(int),1,index_file);
					fwrite(cur_node.key_list,sizeof(int),order-1,index_file);
					fwrite(cur_node.child,sizeof(long),order,index_file);
					l_offset=parent_offset;

					btree_node right_node;
					right_node.key_list=(int *) calloc(order - 1, sizeof(int));
					right_node.child=(long *) calloc(order, sizeof(long));
					right_node.key_num=length-m;
					int temp_count=0;
					//right_node.child[0]=temp_child_list[m];
					for(i=m+1;i<=length;i++)
						{
							right_node.key_list[temp_count]=temp_list[i];
							right_node.child[temp_count]=temp_child_list[i];
							temp_count++;
						}
					right_node.child[temp_count]=temp_child_list[i];

					fseek(index_file,0,SEEK_END); // seek to eof and write
					r_offset=ftell(index_file); 
					fwrite(&(right_node.key_num),sizeof(int),1,index_file);
					fwrite(right_node.key_list,sizeof(int),order-1,index_file);
					fwrite(right_node.child,sizeof(long),order,index_file);


					if(level_count==-1)
					{
						//no parent to this node, any promotion will form a new root
						fseek(index_file,0,SEEK_END);
						root_offset=ftell(index_file);
						btree_node new_root;
						new_root.key_list=(int *) calloc(order - 1, sizeof(int));
						new_root.child=(long *) calloc(order, sizeof(long));
						new_root.key_num=1;
						new_root.child[0]=l_offset;
						new_root.child[1]=r_offset;

						new_root.key_list[0]=temp_list[m];
						//printf("\nnew root val:%d, temp_list[m]:%d\n", new_root.key_list[0],temp_list[m]);
						fwrite(&(new_root.key_num),sizeof(int),1,index_file);
						fwrite(new_root.key_list,sizeof(int),order-1,index_file);
						fwrite(new_root.child,sizeof(long),order,index_file);
						return value;

					}

					
					//there is a parent
					parent_offset=parent_holder[level_count];
					level_count--;
					return insert_promoted( parent_offset, temp_list[m], l_offset, r_offset, index_file);


		}
		else
		{
				//no overflow
		for(i=0;i<=length;i++)
			{
				cur_node.key_list[i]=temp_list[i];	
				cur_node.child[i]=temp_child_list[i];	
			}
				cur_node.child[i]=temp_child_list[i];
				cur_node.key_num=cur_node.key_num+1;
				fseek(index_file,parent_offset,SEEK_SET);
				fwrite(&(cur_node.key_num),sizeof(int),1,index_file);
				fwrite(cur_node.key_list,sizeof(int),order-1,index_file);
				fwrite(cur_node.child,sizeof(long),order,index_file);
				return value;	
		}

		



		return 1;

}

int main(int argc, char const *argv[])
{
	int c;
	FILE *index_file=NULL;
	char input[500],input_copy[500],func[500];
	char *inputParse[2],*input_tokens;
	char *input_file_name;
	int add_key,find_key,j;
	input_file_name=argv[1];
	if((index_file=fopen(input_file_name,"r+b"))==NULL)
	{
		//file doesnt exist, make a new file
		index_file=fopen(argv[1],"w+b");
		//set root offset to -1
		root_offset=-1;
		//write root offset into first element
		//fwrite(&root_offset,sizeof(long),1,index_file);
	}
	else
	{
		//printf("\nfile exists\n");
		fread(&root_offset,sizeof(long),1,index_file);
	}


		//file exists now, get the correct root offset
		

	//printf("\nroot_off:%ld\n",root_offset );
	

	order=atoi(argv[2]);


	while(fgets(input,256,stdin)!=NULL)
	{
	//printf("\n%s\n",input );
	 j=0;	
    strcpy(input_copy,input);

	input_tokens=strtok(input_copy," \n");
	//printf("\n%s\n",input_tokens );
	/*while(input_tokens != NULL)
	{
		inputParse[j++]=input_tokens;
		//printf("\n%s",input_tokens);
		input_tokens=strtok(NULL," ");
		

	}*/
	//printf("\n\n\n\n%s\n\n%s\n\n%s\n",inputParse[0],inputParse[1],inputParse[2]);
	//printf("\n%s\n",inputParse[0] )
//func= strtok(input, " \n");

//printf("\n%s\n",inputParse[0] );
if(input_tokens[0]=='e' && input_tokens[1]=='n' && input_tokens[2]=='d')
		    {
//write root pointer to file
		   //printf("\nin end\n");
		   //printf("\n%s \n",inputParse[0]);
		    fseek(index_file,0,SEEK_SET);
		  	fwrite(&root_offset,sizeof(long),1,index_file);
			return 0;
				}
else if(input_tokens[0]=='a' && input_tokens[1]=='d' && input_tokens[2]=='d')
			{

		//printf("\nIn add\n");
				//printf("\n%s %s\n",inputParse[0],inputParse[1] );
		while(input_tokens != NULL)
	{
		inputParse[j++]=input_tokens;
		//printf("\n%s",input_tokens);
		input_tokens=strtok(NULL," ");
		

	}
		btree_node root_node;
		add_key=atoi(inputParse[1]);
		int ret_val;
		// if(add_key==22)
		// 	printf("\nadding 22\n");
		level_count=-1;
//set the root node
		if(root_offset==-1)
		{	root_offset=sizeof(long);
			fwrite(&root_offset,sizeof(long),1,index_file);
			//printf("\nFirst elem being added\n root_offset:%ld",root_offset);
			//first element being added
			
			root_node.key_num=0;
			root_node.key_list=(int *) calloc(order - 1, sizeof(int));
			root_node.child=(long *) calloc(order, sizeof(long));
			//printf("\nCalling insert\n");
			ret_val=insert_node(add_key,&root_node,index_file);

		}
else
{
	
	//printf("\nnot first elem root_offset:%ld\n",root_offset);
	root_node.key_list=(int *) calloc(order - 1, sizeof(int));
	root_node.child=(long *) calloc(order, sizeof(long));
	fseek(index_file,root_offset,SEEK_SET);
	fread(&(root_node.key_num),sizeof(int),1,index_file);
	fread(root_node.key_list,sizeof(int),(order-1),index_file);
	fread(root_node.child,sizeof(long),order,index_file);
	ret_val=insert_node(add_key,&root_node,index_file);
	
}
	free(root_node.key_list);
	free(root_node.child);
if(ret_val==(-1*add_key))
{
	printf("Entry with key=%d already exists\n",add_key);
}


			}
else if(input_tokens[0]=='f' && input_tokens[1]=='i' && input_tokens[2]=='n')
			{
		//printf("\nIn find\n");
				//printf("\n%s %s\n",inputParse[0],inputParse[1] );
			while(input_tokens != NULL)
	{
		inputParse[j++]=input_tokens;
		//printf("\n%s",input_tokens);
		input_tokens=strtok(NULL," ");
		

	}
		find_key=atoi(inputParse[1]);
		int ret_val=-1;
		if(root_offset!=-1)
			{ret_val=find_node(root_offset,find_key,index_file);}
		if(ret_val==-1)
		{
			printf("Entry with key=%d does not exist\n",find_key);
		}
		else
		{
			printf("Entry with key=%d exists\n",find_key);
		}

			}
else if(input_tokens[0]=='p' && input_tokens[1]=='r' && input_tokens[2]=='i')
			{
//printf("in print\n");
//printf("\n%s \n",inputParse[0]);
int return_print;
return_print= print_tree( root_offset,index_file);

			}	

//while ( (c = getchar()) == '\n' && c != EOF );
}
	
fclose(index_file);
return 0;
}
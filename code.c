#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct node
{
    char val[1000];
    struct node *next;
    struct node *sibling;
};

struct column
{
    char name[200];
    int size;
    struct node *head;
};

struct output
{
    char name[200];
    struct record *values;
};

struct record
{
    char val[1000];
    int sublist;
    int rec_no;
    int flag;
};

struct column *c;

struct node *sortedmerge(struct node *a, struct node *b, int flag, int *cols, int colsSize, int swap_col)
{
    struct node* result = NULL;
 
    if(a == NULL)
        return b;
    else if(b == NULL)
        return a;

    //flag = 0, Ascending
    //flag = 1, Descending

    if(flag == 0)
    {
        if(strcmp(a->val, b->val) < 0)
        {
            result = a;
            result->next = sortedmerge(a->next, b, flag, cols, colsSize, swap_col);
        }
        else if(strcmp(a->val, b->val) > 0)
        {
            result = b;
            result->next = sortedmerge(a, b->next, flag, cols, colsSize, swap_col);
        }
        else
        {
            int i,j,sort_flag=0;
            struct node *temp1, *temp2;
            for(i=1;i<colsSize;++i)
            {
                temp1 = a;
                temp2 = b;
                if(cols[i] == 0)
                {
                    for(j=0;j<swap_col;++j)
                    {
                        temp1 = temp1->sibling;
                        temp2 = temp2->sibling;
                    }
                }
                else
                {
                    for(j=0;j<cols[i];++j)
                    {
                        temp1 = temp1->sibling;
                        temp2 = temp2->sibling;
                    }
                }

                if(strcmp(temp1->val, temp2->val) < 0)
                {
                    result = a;
                    result->next = sortedmerge(a->next, b, flag, cols, colsSize, swap_col);
                    sort_flag = 1;
                    break;
                }
                else if(strcmp(temp1->val, temp2->val) > 0)
                {
                    result = b;
                    result->next = sortedmerge(a, b->next, flag, cols, colsSize, swap_col);
                    sort_flag = 1;
                    break;
                }
            }
            if(sort_flag == 0)
            {                                                               //Values in all fields are equal, then choose first value
                result = a;
                result->next = sortedmerge(a->next, b, flag, cols, colsSize, swap_col);
            }
        }
    }
    else
    {
        if(strcmp(a->val, b->val) > 0)
        {
            result = a;
            result->next = sortedmerge(a->next, b, flag, cols, colsSize, swap_col);
        }
        else if(strcmp(a->val, b->val) < 0)
        {
            result = b;
            result->next = sortedmerge(a, b->next, flag, cols, colsSize, swap_col);
        }
        else
        {
            int i,j,sort_flag=0;
            struct node *temp1, *temp2;
            for(i=1;i<colsSize;++i)
            {
                temp1 = a;
                temp2 = b;
                if(cols[i] == 0)
                {
                    for(j=0;j<swap_col;++j)
                    {
                        temp1 = temp1->sibling;
                        temp2 = temp2->sibling;
                    }
                }
                else
                {
                    for(j=0;j<cols[i];++j)
                    {
                        temp1 = temp1->sibling;
                        temp2 = temp2->sibling;
                    }
                }

                if(strcmp(temp1->val, temp2->val) > 0)
                {
                    result = a;
                    result->next = sortedmerge(a->next, b, flag, cols, colsSize, swap_col);
                    sort_flag = 1;
                    break;
                }
                else if(strcmp(temp1->val, temp2->val) < 0)
                {
                    result = b;
                    result->next = sortedmerge(a, b->next, flag, cols, colsSize, swap_col);
                    sort_flag = 1;
                    break;
                }
            }
            if(sort_flag == 0)
            {
                result = a;
                result->next = sortedmerge(a->next, b, flag, cols, colsSize, swap_col);
            }
        }
    }
    return result;
}

void split_front_back(struct node *s, struct node **front, struct node **back)
{
    struct node* fast;
    struct node* slow;
    if(s == NULL || s->next == NULL)
    {
        *front = s;
        *back = NULL;
    }
    else
    {
        slow = s;
        fast = s->next;
        while (fast != NULL)
        {
            fast = fast->next;
            if (fast != NULL)
            {
                slow = slow->next;
                fast = fast->next;
            }
        }
        *front = s;
        *back = slow->next;
        slow->next = NULL;
    }
}

void mergesort(struct node **head_ref, int flag, int *cols, int colsSize, int swap_col)
{
    struct node *head = *head_ref;
    struct node *a, *b;
 
    if(head == NULL || head->next == NULL)
        return;
 
    split_front_back(head, &a, &b); 
 
    mergesort(&a, flag, cols, colsSize, swap_col);
    mergesort(&b, flag, cols, colsSize, swap_col);
 
    *head_ref = sortedmerge(a, b, flag, cols, colsSize, swap_col); //flag -> Ascending/Descending
}

void swap(struct node *head1, struct node *head2)
{
    if(head1 == head2)
        return;
    
    struct node *temp1 = head1, *temp2 = head2;
    while(temp1 != NULL)
    {
        char temp[1000];

        strcpy(temp, temp1->val);
        strcpy(temp1->val, temp2->val);
        strcpy(temp2->val, temp);

        temp1 = temp1->next;
        temp2 = temp2->next;
    }
}

int main(int argc, char *argv[])
{
    FILE *f, *temp, *out_f;

    struct output *col_out;
    int ct=0,flag,k,k2,j,i,eof_flag,order_flag,col,col_arg[1000];
    long long int memory = atoi(argv[3]);

    if(memory >= 10)
        memory = atoi(argv[3])*1024*1024*0.02;
    else
        memory = atoi(argv[3])*1024*1024;

    long long int r,sum=0,count=0,rec_count=0,file_size;
    char ch,val[10],s[1000];

    f = fopen("metadata.txt","r");
    while((ch = fgetc(f)) != EOF)
    {
        if(ch == '\n')
            ct += 1;
    }
    fclose(f);


    f = fopen("metadata.txt","r");
    c = (struct column *)malloc(sizeof(struct column)*ct);
    col_out = (struct output *)malloc(sizeof(struct output)*ct);
    flag = k = j = ct = eof_flag = k2 = 0;
    while((ch = fgetc(f)) != EOF)
    {
        if(ch != ',' && ch != '\n' && flag == 0)
        {
            c[ct].name[k++] = ch;
            col_out[ct].name[k2++] = ch;
        }
        if(ch == ',')
        {
            flag = 1;
            c[ct].name[k] = '\0';
            col_out[ct].name[k2] = '\0';
            continue;
        }
        if(ch == '\n')
        {
            val[j] = '\0';
            flag = k = j = 0;
            c[ct].size = atoi(val);
            ct += 1;
            continue;
        }
        if(flag == 1)
            val[j++] = ch;
    }
    fclose(f);

    for(i=5;i<argc;++i)
    {
        for(j=0;j<ct;++j)
        {
            if(strcmp(argv[i], c[j].name) == 0)
            {
                col_arg[i-5] = j;
                break;
            }
        }
    }

    for(i=0;i<ct;++i)
        sum += c[i].size;
    r = memory/sum;

    /*
     * Phase 1
     */

    f = fopen(argv[1], "r");
    while(1)
    {
        count = k = 0;
        struct node *prev[ct];
        long long int z = 0; 
        while(1)
        {
            if(count == r)
                break;
            
            if(k == 0)
            {
                int l = 1;
                char ch;
                s[0] = fgetc(f);

                if(feof(f))
                {
                    eof_flag = 1;
                    break;
                }

                while(l != c[k].size)
                {
                    ch = fgetc(f);
                    s[l++] = ch;
                }
                s[l] = '\0';
            }
            else
            {
                if(fscanf(f, "%s", s) == EOF)
                {
                    eof_flag = 1;
                    break;
                }
            }

            int l = strlen(s);
            while(l != c[k].size)
            {
                char ch = fgetc(f);
                s[l++] = ch;
            }
            s[l] = '\0';

            struct node *p = (struct node *)malloc(sizeof(struct node));
            struct node *temp;
            if(c[k].head == NULL)
            {
                strcpy(p->val, s);
                p->next = NULL;
                p->sibling = NULL;
                c[k].head = p;
                prev[k] = c[k].head;
            }
            else
            {
                strcpy(p->val, s);
                p->next = NULL;
                p->sibling = NULL;
                prev[k]->next = p;
                prev[k] = prev[k]->next;
            }
            if(k > 0)
                prev[k-1]->sibling = p;
            
            k = (k+1)%ct;
            if(k == 0)
            {
                fgetc(f);fgetc(f);
                count += 1;
                z += 1;
            }
        }
        
        if(count == 0)
            break;

        col = col_arg[0];    
        if(strcmp(argv[4], "asc") == 0)
            order_flag = 0;
        else if(strcmp(argv[4], "desc") == 0)
            order_flag = 1;

        swap(c[0].head, c[col].head);
        mergesort(&c[0].head, order_flag, col_arg, argc-5, col);
     
        k = 1;
        struct node *t = c[0].head, *t2, *t3;   //Re-structuring after sorting
        while(t != NULL)
        {
            c[k].head = t->sibling;
            t = t->sibling;
            k += 1;
        }

        t3 = c[0].head;
        while(t3->next != NULL)
        {
            t = t3->next;
            t2 = t3->sibling;
            while(t2 != NULL)
            {
                t2->next = t->sibling;
                t = t->sibling;
                t2 = t2->sibling;
            }
            t3 = t3->next;
        }
        while(t3 != NULL)
        {
            t3->next = NULL;
            t3 = t3->sibling;
        }

        swap(c[0].head, c[col].head);  //swap columns to their original places

        temp = fopen("temp.txt", "a+");
        
        struct node *temp1, *temp2;
        temp1 = c[0].head;
        while(temp1 != NULL)
        {
            fprintf(temp, "%s  ", temp1->val);
            temp2 = temp1->sibling;
            while(temp2->sibling != NULL)
            {
                fprintf(temp, "%s  ", temp2->val);
                temp2 = temp2->sibling;
            }
            fprintf(temp, "%s\r\n", temp2->val);
            temp1 = temp1->next;
        }
        fclose(temp);

        rec_count += count; 
        for(i=0;i<ct;++i)
        {
            temp1 = c[i].head;
            while(temp1 != NULL)
            {
                temp2 = temp1->next;
                free(temp1);
                temp1 = temp2;
            }
            c[i].head = NULL;
        }
        
        if(eof_flag == 1)
            break;
    }
    fclose(f);

    /*
     * Phase 2
     */

    f = fopen("temp.txt", "r");
    struct output *out = (struct output *)malloc(sizeof(struct output)*ct);

    for(i=0;i<ct;++i)
    {
        col_out[i].values = (struct record *)malloc(sizeof(struct record)*r);
        out[i].values = (struct record *)malloc(sizeof(struct record)*r);
    }
    
    long long int rec_ct = 0, out_ct = 0;
    int l, subl, r_no,min_idx;
    k = count = subl = r_no = 0;

    while(count < r)
    {
        if(k == 0)
        {
            int l = 1;
            char ch;
            s[0] = fgetc(f);
            
            if(feof(f))
                break;

            while(l != c[k].size)
            {
                ch = fgetc(f);
                s[l++] = ch;
            }
            s[l] = '\0';
        }
        else
            fscanf(f, "%s", s);

        if(feof(f))
            break;

        l = strlen(s);
        while(l != c[k].size)
        {
            char ch = fgetc(f);
            s[l++] = ch;
        }
        s[l] = '\0';

        strcpy(col_out[k].values[count].val, s);
        col_out[k].values[count].sublist = subl;
        col_out[k].values[count].rec_no = r_no;
        col_out[k].values[count].flag = 0;

        k = (k+1)%ct;
        if(k == 0)
        {
            count += 1;
            subl += 1;
            fgetc(f);fgetc(f);
            fseek(f, (sum+6)*(r-1), SEEK_CUR);
        }
    }
    
    int x = 0;
    long long int rec_read = 0;
    eof_flag = 0;
    while(1)
    {
        if(eof_flag == 0)
        {
            for(i=0;i<count;++i)
            {
                if(col_out[col].values[i].flag == 0)
                {
                    min_idx = i;
                    break;
                }
            }

            for(i=1;i<count;++i)
            {
                if(order_flag == 0)
                {
                    if(col_out[col].values[i].flag == 0 && strcmp(col_out[col].values[i].val, col_out[col].values[i-1].val) < 0)
                        min_idx = i;
                    else if(col_out[col].values[i].flag == 0 && strcmp(col_out[col].values[i].val, col_out[col].values[i-1].val) == 0)
                    {
                        for(j=0;j<argc-5;++j)
                        {
                            if(col_out[j].values[i].flag == 0 && strcmp(col_out[j].values[i].val, col_out[j].values[i-1].val) < 0)
                            {
                                min_idx = i;
                                break;
                            }
                        }
                    }
                }
                else if(order_flag == 1)
                {
                    if(col_out[col].values[i].flag == 0 && strcmp(col_out[col].values[i].val, col_out[col].values[i-1].val) > 0)
                        min_idx = i;
                    else if(col_out[col].values[i].flag == 0 && strcmp(col_out[col].values[i].val, col_out[col].values[i-1].val) == 0)
                    {
                        for(j=0;j<argc-5;++j)
                        {
                            if(col_out[j].values[i].flag == 0 && strcmp(col_out[j].values[i].val, col_out[j].values[i-1].val) > 0)
                            {
                                min_idx = i;
                                break;
                            }
                        }
                    }
                }
            }
        
            for(i=0;i<ct;++i)
                strcpy(out[i].values[x].val, col_out[i].values[min_idx].val);

            rec_ct += 1;
            rec_read += 1;

            x = (x+1)%r;
        }

        if(x == 0 || eof_flag == 1 || rec_ct == rec_count)
        {
            out_f = fopen(argv[2], "a+");
            for(i=0;i<rec_read;++i)
            {
                for(j=0;j<ct-1;++j)
                    fprintf(out_f, "%s  ", out[j].values[i].val);
                fprintf(out_f, "%s\r\n", out[j].values[i].val);
            }
            fclose(out_f);
            rec_read = 0;
            x = 0;
        }
        
        if(rec_ct == rec_count)
            break;

        r_no = col_out[col].values[min_idx].rec_no + 1;
        if(r_no == r || eof_flag == 1)
        {
            col_out[col].values[min_idx].flag = -1;
            eof_flag = 0;
            continue;
        }

        subl = col_out[col].values[min_idx].sublist;
        fseek(f, (sum+6)*(subl*r+r_no), SEEK_SET);

        k = 0;
        while(k < ct)
        {
            if(k == 0)
            {
                int l = 1;
                char ch;
                s[0] = fgetc(f);
                
                if(feof(f))
                {
                    eof_flag = 1;
                    break;
                }

                while(s[0] == '\r' || s[0] == '\n')
                    s[0] = fgetc(f);

                while(l != c[k].size)
                {
                    ch = fgetc(f);
                    s[l++] = ch;
                }
                s[l] = '\0';
            }
            else
                fscanf(f, "%s", s);

            if(feof(f))
            {
                eof_flag = 1;
                break;
            }

            l = strlen(s);
            while(l != c[k].size)
            {
                char ch = fgetc(f);
                s[l++] = ch;
            }
            s[l] = '\0';

            strcpy(col_out[k].values[min_idx].val, s);
            col_out[k].values[min_idx].sublist = subl;
            col_out[k].values[min_idx].rec_no = r_no;
            col_out[k].values[min_idx].flag = 0;

            k = k+1;
        }
    }
    fclose(f);
    return 0;
}

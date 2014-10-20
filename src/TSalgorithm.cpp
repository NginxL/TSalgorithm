#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <ctime>
#include <cstdlib>

using namespace std;

#define M 13
#define N 13

double huliyinzi[M][N] = { 0 };
double linjiejuzhen[M][N] = { 0 };

struct node{
	int edge[2];
	double zuiduanjuli[13][13];
	double result;
};

//领域表节点
struct mid{
	int move[2];
	double change_value;
	int add;
};

//禁忌表节点
struct forb{
	int move[2];
};


//禁止删除表的节点
struct forb2{
	int move[2];
};

vector<node> result;	//结果集
map<int, int> out;	//顶点出度
map<int, int> in;	//顶点入度
vector<mid> mid_value(10);	//邻域表
vector<forb> forbid;	//禁忌表
vector<forb2> forbid2;	//禁止删除的边

int old_best_count = 0;

bool comp(struct mid& a,struct mid& b){
	return a.change_value > b.change_value;
}

void file_read_huliyinzi(){
	ifstream ifile;
	ifile.open("huliyinzi.txt");
	string s;
	int row = 1;
	int col = 1;
	while (getline(ifile, s)){
		istringstream iss(s);
		double temp;
		while (iss>>temp){
			huliyinzi[row][col] = temp;
			col++;
		}
		row++;
		col = 1;
	}
}

void file_read_linjiejuzhen(){
	ifstream ifile;
	ifile.open("linjiejuzhen.txt");
	string s;
	int row = 1;
	int col = 1;
	while (getline(ifile, s)){
		istringstream iss(s);
		double temp;
		while (iss >> temp){
			linjiejuzhen[row][col] = temp;
			col++;
		}
		row++;
		col = 1;
	}
}

void Dijkstra(const int startVertex,double map[13][13],int* distance,int* prevVertex){
	vector<bool> isInS;
	isInS.reserve(0);
	isInS.assign(13,false);

	for (int i = 1; i <= 12; i++){
		distance[i] = map[startVertex][i];
		if (map[startVertex][i] < INT_MAX)
			prevVertex[i] = startVertex;
		else
			prevVertex[i] = -1;
	}
	prevVertex[startVertex] = -1;

	isInS[startVertex] = true;
	
	int u = startVertex;
	
	for (int i = 1; i < 12; i++){
		int nextVertex = u;
		int tempDistance = INT_MAX;
		for (int j = 1; j <= 12; j++){
			if ((isInS[j] == false) && (distance[j] < tempDistance)){
				nextVertex = j;
				tempDistance = distance[j];
			}
		}
		isInS[nextVertex] = true;
		u = nextVertex;
		for (int j = 1; j <= 12; j++){
			if (isInS[j] == false && map[u][j] < INT_MAX){
				int temp = distance[u] + map[u][j];
				if (temp < distance[j]){
					distance[j] = temp;
					prevVertex[j] = u;
				}
			}
		}
	}
}

double matrix_multi(double matrix1[13][13], double matrix2[13][13]){
	double sum = 0;
	for (int i = 1; i <= 12; i++){
		for (int j = 1; j <= 12; j++){
			sum = sum + matrix1[i][j] * matrix2[i][j];
		}
	}
	return sum;
}

double random(double start, double end)
{
	return start + (end - start)*rand() / (RAND_MAX + 1.0);
}

bool search_forbid2(int start_edge, int end_edge){
	for (int i = 0; i != forbid.size();i++){
		if (forbid2[i].move[0] == start_edge && forbid2[i].move[1] == end_edge){
			return true;
		}
	}
	return false;
}

bool search_linjiejuzhen(int start_edge, int end_edge){
	if (linjiejuzhen[start_edge][end_edge] == 1)
		return true;
	else
		return false;
}

bool search_forbid(int start_edge,int end_edge){
	for (int i = 0; i != forbid.size(); i++){
		if (forbid[i].move[0] == start_edge && forbid[i].move[1] == end_edge)
			return true;
	}
	return false;
}

void add_forbid(int start_edge,int end_edge){
	if (forbid.size() == 3){
		forbid[0].move[0] = forbid[1].move[0];
		forbid[0].move[1] = forbid[1].move[1];
		forbid[1].move[0] = forbid[1].move[0];
		forbid[1].move[1] = forbid[1].move[1];
		forbid[2].move[0] = start_edge;
		forbid[2].move[1] = end_edge;
	}
	else{
		struct forb temp_forb;
		temp_forb.move[0] = start_edge;
		temp_forb.move[1] = end_edge;
		forbid.push_back(temp_forb);
	}
}

int main(int argc,char** argv){
	file_read_huliyinzi();
	file_read_linjiejuzhen();

	for (int i = 1; i <= 12; i++){
		for (int j = 1; j <= 12; j++){
			if (i != j && linjiejuzhen[i][j] == 0){
				linjiejuzhen[i][j] = INT_MAX;
			}
		}
	}

	int distance[13];
	int prevVertex[13];
	struct node temp_node;
	for (int i = 1; i <= 12; i++){
		Dijkstra(i, linjiejuzhen, distance, prevVertex);
		for (int j = 1; j <= 12; j++){
			temp_node.zuiduanjuli[i][j] = distance[j];
		}
	}
	temp_node.edge[0] = 0;
	temp_node.edge[1] = 0;
	temp_node.result = matrix_multi(temp_node.zuiduanjuli,huliyinzi);
	result.push_back(temp_node);
	
	double current_best = temp_node.result;
	int current_index = 0;
	double old_best = current_best;
	int old_index;

	ifstream ifilee;
	ifilee.open("adddelete.txt");
	string ss;
	int a[2];
	while (getline(ifilee,ss)){
		istringstream is(ss);
		int temp1;
		int temp2;
		forb2 temp3;
		while (is >> temp1 >> temp2){
			temp3.move[0] = temp1;
			temp3.move[1] = temp2;
		}
		forbid2.push_back(temp3);
	}

	for (int i = 1; i <= 12; i++){
		out[i] = 0;
		in[i] = 0;
	}
	for (int i = 1; i <= 12; i++){
		for (int j = 1; j <= 12; j++){
			if (linjiejuzhen[i][j] == 1){
				out[i]++;
				in[j]++;
			}
		}
	}

	forbid.reserve(0);
	forbid2.reserve(0);

	srand(unsigned(time(0)));
	for (int i = 1; i <= 5; i++){
		while (true){
			int start_edge = int(random(1, 12));
			int end_edge = int(random(1, 12));
			if (start_edge == end_edge){
				continue;
			}
			if (search_linjiejuzhen(start_edge,end_edge)){
				continue;
			}
			if (out[start_edge] >= 3 || in[end_edge] >= 3){
				continue;
			}
			linjiejuzhen[start_edge][end_edge] = 1;
			double temp_matrix[13][13] = {0};
			for (int i = 1; i <= 12; i++){
				Dijkstra(i,linjiejuzhen,distance,prevVertex);
				for (int j = 1; j <= 12; j++){
					temp_matrix[i][j] = distance[j];
				}
			}
			double temp_value = matrix_multi(temp_matrix,huliyinzi);
			double temp = temp_value - current_best;
			struct mid temp_mid;
			temp_mid.change_value = temp;
			temp_mid.move[0] = start_edge;
			temp_mid.move[1] = end_edge;
			temp_mid.add = 1;
			mid_value.push_back(temp_mid);
			linjiejuzhen[start_edge][end_edge] = 0;
			break;
		}

		while (true){
			int start_edge = int(random(1, 12));
			int end_edge = int(random(1, 12));
			if (start_edge == end_edge){
				continue;
			}
			if (!search_linjiejuzhen(start_edge, end_edge)){
				continue;
			}
			if (search_forbid2(start_edge,end_edge)){
				continue;
			}
			if (out[start_edge] >= 3 || in[end_edge] >= 3){
				continue;
			}
			linjiejuzhen[start_edge][end_edge] = 1;
			double temp_matrix[13][13] = { 0 };
			for (int i = 1; i <= 12; i++){
				Dijkstra(i, linjiejuzhen, distance, prevVertex);
				for (int j = 1; j <= 12; j++){
					temp_matrix[i][j] = distance[j];
				}
			}
			double temp_value = matrix_multi(temp_matrix, huliyinzi);
			double temp = temp_value - current_best;
			struct mid temp_mid;
			temp_mid.change_value = temp;
			temp_mid.move[0] = start_edge;
			temp_mid.move[1] = end_edge;
			temp_mid.add = 0;
			mid_value.push_back(temp_mid);
			linjiejuzhen[start_edge][end_edge] = 0;
			break;
		}
	}
	sort(mid_value.begin(),mid_value.end(),comp);

	for (int i = 0; i != 1000; i++){
		int start_edge;
		int end_edge;
		struct mid temp_mid;
		for (int i = 0; i != mid_value.size(); i++){
			temp_mid = mid_value[i];
			start_edge = temp_mid.move[0];
			end_edge = temp_mid.move[1];
			if (search_forbid(start_edge, end_edge) && current_best + temp_mid.change_value < old_best){
				continue;
			}
			break;
		}
		if (temp_mid.add == 1){
			linjiejuzhen[start_edge][end_edge] = 1;
		}
		else{
			linjiejuzhen[start_edge][end_edge] = 0;
		}
		current_best = current_best + temp_mid.change_value;
		struct node temp_node;
		temp_node.edge[0] = start_edge;
		temp_node.edge[1] = end_edge;
		temp_node.result = current_best;
		for (int i = 1; i <= 12; i++){
			Dijkstra(i,linjiejuzhen,distance,prevVertex);
			for (int j = 1; j <= 12; j++){
				temp_node.zuiduanjuli[i][j] = distance[j];
			}
		}
		result.push_back(temp_node);
		current_index++;
		if (current_best>old_best){
			old_best = current_best;
			old_best_count++;
		}

		add_forbid(start_edge,end_edge);

		for (int i = 1; i <= 5; i++){
			while (true){
				int start_edge = int(random(1, 12));
				int end_edge = int(random(1, 12));
				if (start_edge == end_edge){
					continue;
				}
				if (search_linjiejuzhen(start_edge, end_edge)){
					continue;
				}
				if (out[start_edge] >= 3 || in[end_edge] >= 3){
					continue;
				}
				linjiejuzhen[start_edge][end_edge] = 1;
				double temp_matrix[13][13] = { 0 };
				for (int i = 1; i <= 12; i++){
					Dijkstra(i, linjiejuzhen, distance, prevVertex);
					for (int j = 1; j <= 12; j++){
						temp_matrix[i][j] = distance[j];
					}
				}
				double temp_value = matrix_multi(temp_matrix, huliyinzi);
				double temp = temp_value - current_best;
				struct mid temp_mid;
				temp_mid.change_value = temp;
				temp_mid.move[0] = start_edge;
				temp_mid.move[1] = end_edge;
				temp_mid.add = 1;
				mid_value.push_back(temp_mid);
				linjiejuzhen[start_edge][end_edge] = 0;
				break;
			}

			while (true){
				int start_edge = int(random(1, 12));
				int end_edge = int(random(1, 12));
				if (start_edge == end_edge){
					continue;
				}
				if (!search_linjiejuzhen(start_edge, end_edge)){
					continue;
				}
				if (search_forbid2(start_edge, end_edge)){
					continue;
				}
				if (out[start_edge] >= 3 || in[end_edge] >= 3){
					continue;
				}
				linjiejuzhen[start_edge][end_edge] = 1;
				double temp_matrix[13][13] = { 0 };
				for (int i = 1; i <= 12; i++){
					Dijkstra(i, linjiejuzhen, distance, prevVertex);
					for (int j = 1; j <= 12; j++){
						temp_matrix[i][j] = distance[j];
					}
				}
				double temp_value = matrix_multi(temp_matrix, huliyinzi);
				double temp = temp_value - current_best;
				struct mid temp_mid;
				temp_mid.change_value = temp;
				temp_mid.move[0] = start_edge;
				temp_mid.move[1] = end_edge;
				temp_mid.add = 0;
				mid_value.push_back(temp_mid);
				linjiejuzhen[start_edge][end_edge] = 0;
				break;
			}
		}
		sort(mid_value.begin(), mid_value.end(), comp);
	}
	cout << old_best << endl;
	return 0;
}
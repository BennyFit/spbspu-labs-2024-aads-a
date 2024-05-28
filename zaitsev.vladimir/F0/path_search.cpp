#include "path_search.hpp"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <forward_list>
#include <unordered_map>
#include <limits>
#include "stream_guard.hpp"

using std::forward_list;
using std::vector;
using std::pair;
using std::string;
using std::unordered_map;

constexpr int inf = std::numeric_limits< int >::max();

struct edge
{
  size_t a;
  size_t b;
  int val;
};

unordered_map< string, size_t > convert_to_indexes(const zaitsev::graph_t& graph);
vector<vector<int>> create_adjacency_matrix(const zaitsev::graph_t& graph);
vector< edge > extract_edges(const zaitsev::graph_t& graph);
vector< vector< int > > calc_paths_floyd(const vector< vector< int > >& matrix);
pair< vector< int >, vector< size_t >  > calc_paths_ford(const vector< edge >& edges, size_t begin, size_t vert_nmb);

void zaitsev::shortest_distance(const base_t& graphs, std::istream& in, std::ostream& out)
{
  string graph, begin, end;
  in >> graph >> begin >> end;
  if (!in)
  {
    throw std::ios::failure("Input error");
  }
  base_t::const_iterator graph_pos = graphs.find(graph);
  if (graph_pos == graphs.end())
  {
    throw std::invalid_argument("Graph doesn't exist");
  }
  graph_t::const_iterator graph_beg = graph_pos->second.find(begin);
  graph_t::const_iterator graph_end = graph_pos->second.find(end);
  if (graph_beg == graph_pos->second.end() || graph_end == graph_pos->second.end())
  {
    throw std::invalid_argument("Vertex doesn't exist");
  }
  unordered_map< string, size_t > indexes = convert_to_indexes(graph_pos->second);
  vector< edge > edges = extract_edges(graph_pos->second);
  pair< vector< int >, vector< size_t > > dist_with_prev = calc_paths_ford(edges, indexes[begin], indexes.size());

  if (dist_with_prev.first[indexes[begin]] == inf)
  {
    throw std::invalid_argument("Graph contains negative weight cycles");
  }
  if (dist_with_prev.first[indexes[begin]] == inf)
  {
    out << "Vertex \"" << end << "\" is unreachable from \"" << begin << "\".\n";
  }
  else
  {
    out << dist_with_prev.first[indexes[end]] << '\n';
  }
  return;
}

void zaitsev::shortest_path_trace(const base_t& graphs, std::istream& in, std::ostream& out)
{
  string graph_name, begin_name, end_name;
  in >> graph_name >> begin_name >> end_name;
  if (!in)
  {
    throw std::ios::failure("Input error");
  }
  base_t::const_iterator graph_pos = graphs.find(graph_name);
  if (graph_pos == graphs.end())
  {
    throw std::invalid_argument("Graph doesn't exist");
  }
  const graph_t& graph = graph_pos->second;
  if (graph.find(begin_name) == graph.end() || graph.find(end_name) == graph.end())
  {
    throw std::invalid_argument("Vertex doesn't exist");
  }
  unordered_map< string, size_t > indexes = convert_to_indexes(graph);
  vector< edge > edges = extract_edges(graph);
  pair< vector< int >, vector< size_t > > dist_with_prev = calc_paths_ford(edges, indexes[begin_name], indexes.size());

  if (dist_with_prev.first[indexes[begin_name]] == inf)
  {
    throw std::invalid_argument("Graph contains negative weight cycles");
  }
  if (dist_with_prev.first[indexes[begin_name]] == inf)
  {
    out << "Vertex \"" << end_name << "\" is unreachable from \"" << begin_name << "\".\n";
  }
  else
  {
    forward_list< string >path = { end_name };
    size_t i = indexes[end_name];
    while (path.front() != begin_name)
    {
      i = dist_with_prev.second[i];
      path.push_front(std::next(graph.begin(), i)->first);
    }
    out << path.front();
    for (auto i = ++path.begin(); i != path.end(); ++i)
    {
      out << "  " << *i;
    }
    out << '\n';
  }
  return;
}

void zaitsev::shortest_paths_matrix(const base_t& graphs, std::istream& in, std::ostream& out)
{
  string graph_name;
  in >> graph_name;
  base_t::const_iterator it = graphs.find(graph_name);
  if (it == graphs.end())
  {
    throw std::invalid_argument("Graph with name \"" + graph_name + "\", doesn't exists.");
  }
  vector< vector< int > > adj_matrix = create_adjacency_matrix(it->second);
  vector< vector< int > > distances = calc_paths_floyd(adj_matrix);


  size_t max_int_len = std::to_string(std::numeric_limits< int >::lowest()).size();
  auto get_len = [](const std::pair< string, unit_t >& a)
    {
      return a.first.size();
    };
  auto need_to_extend = [&](const size_t len)
    {
      return len < max_int_len;
    };



  vector< size_t > names_length;
  std::transform(it->second.begin(), it->second.end(), std::back_inserter(names_length), get_len);
  size_t names_column_width = *(std::max_element(names_length.begin(), names_length.end()));
  std::replace_if(names_length.begin(), names_length.end(), need_to_extend, max_int_len);
  string names_indent(names_column_width, ' ');
  string indent(2, ' ');
  StreamGuard guard(out);

  out << names_indent;
  for (graph_t::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
  {
    out << indent << i->first;
  }
  out << '\n';
  size_t i = 0;
  for (graph_t::const_iterator ii = it->second.begin(); ii != it->second.end(); ++ii)
  {
    out << std::left << std::setw(names_column_width) << ii->first;
    for (auto j : distances[i])
    {
      out << indent << std::left << std::setw(names_length[i]) << j;
    }
    out << '\n';
    ++ii;
  }
  return;
}

void zaitsev::check_negative_weight_cycles(const base_t& graphs, std::istream& in, std::ostream& out)
{
  string graph_name;
  in >> graph_name;
  base_t::const_iterator it = graphs.find(graph_name);
  if (it == graphs.end())
  {
    throw std::invalid_argument("Graph with name \"" + graph_name + "\", doesn't exists.");
  }
  vector< vector< int > > adj_matrix = create_adjacency_matrix(it->second);
  vector< vector< int > > distances = calc_paths_floyd(adj_matrix);
  bool negative_cycles = false;
  for (size_t i = 0; i < distances.size(); ++i)
  {
    if (distances[i][i] < 0)
    {
      negative_cycles = true;
      break;
    }
  }
  out << "Graph " << (negative_cycles ? "contains" : "doesn't contain") << " a negative weight cycle\n";
}

unordered_map<string, size_t> convert_to_indexes(const zaitsev::graph_t& graph)
{
  using namespace zaitsev;
  unordered_map< string, size_t > vert_indexes(graph.size());
  size_t index = 0;
  for (graph_t::const_iterator i = graph.begin(); i != graph.end(); ++i, ++index)
  {
    vert_indexes[i->first] = index;
  }
  return vert_indexes;
}

vector<vector<int>> create_adjacency_matrix(const zaitsev::graph_t& graph)
{
  using namespace zaitsev;
  vector<vector<int>> matrix(graph.size(), vector<int>(graph.size(), inf));
  unordered_map< string, size_t > vert_indexes = convert_to_indexes(graph);

  size_t i = 0;
  for (graph_t::const_iterator it_i = graph.begin(); it_i != graph.end(); ++it_i)
  {
    for (unit_t::const_iterator it_j = it_i->second.begin(); it_j != it_i->second.end(); ++it_j)
    {
      matrix[i][vert_indexes[it_j->first]] = it_j->second;
    }
    ++i;
  }
  return matrix;
}

vector< edge > extract_edges(const zaitsev::graph_t& graph)
{
  using namespace zaitsev;

  unordered_map< string, size_t > vert_indexes = convert_to_indexes(graph);
  size_t edges_nmb = 0;
  for (graph_t::const_iterator i = graph.begin(); i != graph.end(); ++i)
  {
    edges_nmb += i->second.size();
  }
  vector< edge > edges_list(edges_nmb);
  size_t i = 0, k = 0;
  for (graph_t::const_iterator it_i = graph.begin(); it_i != graph.end(); ++it_i)
  {
    for (unit_t::const_iterator it_j = it_i->second.begin(); it_j != it_i->second.end(); ++it_j)
    {
      edges_list[k] = { i, vert_indexes[it_j->first], it_j->second };
      ++k;
    }
    ++i;
  }
  return edges_list;
}

pair< vector< int >, vector< size_t > > calc_paths_ford(const vector< edge >& edges, size_t begin, size_t vert_nmb)
{
  vector< int > dist(vert_nmb, inf);
  dist[begin] = 0;
  vector< size_t > prev(vert_nmb, vert_nmb + 1);
  bool changed = true;
  size_t phase_nmb = 0;
  while (changed && phase_nmb < vert_nmb + 1)
  {
    changed = false;
    for (size_t j = 0; j < edges.size(); ++j)
    {
      if (dist[edges[j].a] < inf && dist[edges[j].b] > dist[edges[j].a] + edges[j].val)
      {
        dist[edges[j].b] = dist[edges[j].a] + edges[j].val;
        prev[edges[j].b] = edges[j].a;
        changed = true;
      }
    }
    ++phase_nmb;
  }
  if (phase_nmb == vert_nmb + 1)
  {
    std::fill(dist.begin(), dist.end(), inf);
  }

  return { dist, prev };
  /* if (d[t] == inf)
     cout << "No path from " << v << " to " << t << ".";
   else {
     vector<int> path;
     for (int cur = t; cur != -1; cur = p[cur])
       path.push_back(cur);
     reverse(path.begin(), path.end());

     cout << "Path from " << v << " to " << t << ": ";
     for (size_t i = 0; i < path.size(); ++i)
       cout << path[i] << ' ';
   }*/
}

vector< vector< int > > calc_paths_floyd(vector< vector< int > >& matrix)
{
  for (size_t k = 0; k < matrix.size(); ++k)
  {
    for (size_t i = 0; i < matrix.size(); ++i)
    {
      for (size_t j = 0; j < matrix.size(); ++j)
      {
        if (matrix[i][k] < inf && matrix[k][j] < inf)
        {
          matrix[i][j] = std::min(matrix[i][j], matrix[i][k] + matrix[k][j]);
        }
      }
    }
  }
  return matrix;
}
